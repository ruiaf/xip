/*
   Copyright (c) 2003-2005
   Rui Ferreira, Ricardo Barreira, Nuno Cerqueira, Rui Martins
   University of Porto
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   3. Neither the name of the University of Porto nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include "Meditator.h"

#define MAX_DOCK_TIME 20
#define MOUSE_DISTACE_CYCLES_EFF (1.4)
#define MOUSE_DISTACE_CYCLES_VEFF (1.2)
#define TIME_TO_END_MARGIN (300)
#define VNEAR_TO_BEACON (50)

Meditator::Meditator(WorldState *w, MotorController *m)
{
	ws=w;
	motor=m;
	behaveState=SEARCH_BEACON;
	wantedToReturn = false;
	toAbort=false;
	goingThroughKnownPlaces=true;
	notReachedCount=0;
	knowWellWayBetweenObjectives=false;
	beingEfficient = false;
	beingVeryEfficient = false;

	estimatedCyclesEndKnownPlaces=-1;
	estimatedCyclesEndUnKnownPlaces=-1;
	numberOfCyclesToBeacon=-1;

	returnBeacon = NULL; 
	planner = new Plan(ws,motor);
}

SensorRequest Meditator::CalculateSensorRequest() {
	SensorRequest r = SensorRequest();
	static int typea_time=-1,typeb_time=0;
	if ((ws->getSensors()->hasHighRisk() && abs(typea_time-typeb_time)<2) || (typeb_time>=typea_time && ws->getTime()>0)) {
		memset(r.ir,-1,sizeof(r.ir));
		typea_time=ws->getTime();
	} else {
		r.compass=true;
		r.ground=true;
		int cb=0;
		for (int i=0;i<ws->getParameters()->getNBeacons() && cb<=2;i++) {
			if (!ws->getBeacon(i)->hasBeenVisited())
				{
					r.beacon[i]=true;
					cb++;
				}
		}
		typeb_time=ws->getTime();
	}
	return r;
}

Action * Meditator::bestAction()
{
	Action *act = NULL;  

	if (ws->getTime()==0)
		puts("Searching for beacon");


	if (ws->getSensors()->inCollision())
		puts("Colided!");

	switch(behaveState)
	{

		case SEARCH_BEACON:
			// se a ult accao n foi enviada já gastamos algum do tempo de processamento
			// para este ciclo, Mais vale enviar uma acção apenas reactiva e sincronizar o processamento
			if (!ws->ActionSent())
			{
				ws->setPlan(NULL);
				puts("Sync");
				return new Action(ws->getMotors()->getInertiaLeft(),ws->getMotors()->getInertiaRight());
			}

			// comportamento de pesquisa de farol
			act = actionSearchBeacon();
			if (ws->getPlan() != NULL &&
				!ws->getPlan()->ReachedEnd()) // caso nao tenha conseguido encontrar caminho para o destino
			{
				ws->getBeaconsearch()->markCurrentPositionAsUnreachable();
				// se todos os pontos forem marcados como livres
				if (ws->getBeaconsearch()->allUnreachableOrVisited()) 
					ws->getMaze()->erodeMazeToUnknown(); // causa erosão às paredes
				// indica ao farol que este não pode estar no ponto indicado
				else {
					for (int i=0 ; i<ws->getParameters()->getNBeacons();i++)
						ws->getBeacon(i)->markCurrentPositionAsUnreachable();  
				}
			}
			// caso tenha um rato a obstruir o caminho...
			if (motor->wasActionCorrected() && motor->isMouseStoppedAhead())
				ws->getMaze()->PaintMouseStoppedAhead();
			break;

		case GOTO_BEACON:
			// comportamente de movimento para o farol

			// se a ult acção n foi enviada já gastamos algum do tempo de processamento
			// para este ciclo, Mais vale enviar uma acção apenas reactiva e sincronizar o processamento
			if (!ws->ActionSent())
			{
				ws->setPlan(NULL);
				puts("Sync");
				return new Action(ws->getMotors()->getInertiaLeft(),ws->getMotors()->getInertiaRight());
			}

			act = actionGoToBeacon();
			if (ws->getPlan() != NULL &&
				!ws->getPlan()->ReachedEnd()) // caso não tenha conseguido encontrar caminho para o destino
				for (int i=0 ; i<ws->getParameters()->getNBeacons();i++)
						ws->getBeacon(i)->markCurrentPositionAsUnreachable();

			if (!reachedEndinLastCycles()) // todos os caminhos estão obstruidos
			{
				ws->getMaze()->erodeMazeToUnknown(); // causa erosão às paredes
			}
			// caso tenha um rato (objecto?) a obstruir o caminho...
			if (motor->wasActionCorrected() && motor->isMouseStoppedAhead())
				ws->getMaze()->PaintMouseStoppedAhead();
			break;

		case GOTO_RETURN_BEACON:
			// TODO: 2009: PENSO QUE ESTA ACCAO DEIXA DE EXISTIR. ACABA POR SER BOM PORQUE ESTA
			//				É A PARTE MAIS PESADA DO CÓDIGO - PASSA A SER NECESSÁRIO APENAS 1 A* :)
			// se a ult acção n foi enviada já gastamos algum do tempo de processamento
			// para este ciclo, Mais vale enviar uma acção apenas reactiva e sincronizar o processamento
			if (!ws->ActionSent())
			{
				ws->setPlan(NULL);
				puts("Sync");
				return new Action(ws->getMotors()->getInertiaLeft(),ws->getMotors()->getInertiaRight());
			}

			act = actionGoToReturnBeacon();

			// caso tenha um rato (objecto?) a obstruir o caminho...
			if (motor->wasActionCorrected() && motor->isMouseStoppedAhead())
				ws->getMaze()->PaintMouseStoppedAhead();
			break;

		case GOTO_START:
			// TODO: 2009: PENSO QUE ESTA ACCAO DEIXA DE EXISTIR. ACABA POR SER BOM PORQUE ESTA
			//				É A PARTE MAIS PESADA DO CÓDIGO - PASSA A SER NECESSÁRIO APENAS 1 A* :)
			// ENVIAR ANTES PARA O PONTO ONDE CAUSA MENOS PROBLEMAS E NÃO GASTA PROCESSAMENTO

			// comportamento de movimento para o ponto inicial
			if (wantedToReturn && !ws->ActionSent()) // falha no envio do pacote
			{
				puts("Falhou a acender o LED de retorno");
				behaveState = GOTO_BEACON;
				wantedToReturn = false;
				puts("sync");
				ws->setPlan(NULL);
				act = new Action(-ws->getMotors()->getInertiaLeft(),-ws->getMotors()->getInertiaRight());
			}
			if (!ws->ActionSent())
			{
				puts("Falhou a enviar acção");
				wantedToReturn = false;
				puts("sync");
				ws->setPlan(NULL);
				act = new Action(ws->getMotors()->getInertiaLeft(),ws->getMotors()->getInertiaRight());
			}
			else // caso normal
			{
				act = actionGoToStart();
				wantedToReturn = false;
			}
			// caso tenha um rato (objecto?) a obstruir o caminho...
			if (motor->wasActionCorrected() && motor->isMouseStoppedAhead())
				ws->getMaze()->PaintMouseStoppedAhead();
			// caso não exista caminho por sitios conhecidos... explora.
			if (ws->getPlan()!=NULL && !ws->getPlan()->ReachedEnd() && goingThroughKnownPlaces)
				goingThroughKnownPlaces = false;
			// caso não exista mesmo caminho... erode as paredes....
			else if (ws->getPlan()!=NULL && !reachedEndinLastCycles())
				ws->getMaze()->erodeMazeToUnknown();
			break;
	}

	// actualiza o contador de ciclos sem atingir o objectivo
	if (ws->getPlan()==NULL || ws->getPlan()->ReachedEnd())
		notReachedCount--;
	else notReachedCount+=2;

	if (notReachedCount < 0) notReachedCount=0;
	
	return act;
} 

Action * Meditator::actionSearchBeacon()
{
	Action *act;

	if ((act = dealWithCollision()) != NULL) return act;

	for (int i=0; i<ws->getParameters()->getNBeacons(); i++)
	{
		if (!ws->getBeacon(i)->hasBeenVisited() && ws->getBeacon(i)->getProb())
		{
			behaveState = GOTO_BEACON;
			puts("Found beacon!");
			return actionGoToBeacon();
		}
	}

	ws->setPlan(NULL);
	act = planner->wayTo(ws->getBeaconsearch()->getProbableBeaconPos());
	ws->setPlan(planner);
	return act;
}

Action * Meditator::actionGoToBeacon()
{
	Action *act;

	if ((act = dealWithCollision()) != NULL) return act;

	ws->setPlan(NULL);
	WSbeacon * beaconToGo = NULL;
	bool knownAll = true;

	for (int i=0; i<ws->getParameters()->getNBeacons(); i++)
	{
		if (!ws->getBeacon(i)->hasBeenVisited())
		{
			knownAll = false;
			if (ws->getBeacon(i)->getProb())
			{
				if (beaconToGo == NULL ||
						(ws->getBeacon(i)->getPosition().distance(ws->getPosition()->getPos())
						 < beaconToGo->getPosition().distance(ws->getPosition()->getPos()))) {
					beaconToGo = ws->getBeacon(i);
				}
			}
		}
	}

	if (knownAll)
	{
		behaveState = GOTO_START;
		act = actionGoToStart();
		act->setType(RETURN); 
		wantedToReturn = true;
		return act;
	}
	else if (!beaconToGo)
	{
		behaveState = SEARCH_BEACON;
		puts("Have to search for other beacons");
		return actionSearchBeacon();	
	}

	if (ws->getSensors()->inGround(beaconToGo->getNumber()))
	{
		beaconToGo->setVisited();
		puts("Going to next beacon");
		act = new Action(0.0,0.0);
		act->setType(VISIT,beaconToGo->getNumber());
		foundTime=0;
		return act;
	}

	//  beaconToGo->getPosition().print();
	act = planner->wayTo(beaconToGo->getPosition());
	ws->setPlan(planner);
	numberOfCyclesToBeacon = (int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF);

	return act;
}

Action * Meditator::actionGoToReturnBeacon()
{
	Action *act;
	if ((act = dealWithCollision()) != NULL) return act;

	decideBeaconToReturn();	

	ws->setPlan(NULL);
	act = planner->wayThroughKnownPlacesTo(returnBeacon->getPosition());
	ws->setPlan(planner);
	numberOfCyclesToBeacon = (int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF);

	bool haveExtraTime = (ws->getParameters()->getSimTime()-ws->getTime()) >=
		(numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_EFF + TIME_TO_END_MARGIN);
	bool haveTime = !haveExtraTime && ((ws->getParameters()->getSimTime()-ws->getTime()) >=
		numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_VEFF);

	char debug[255];
	sprintf(debug,"MEDITATOR - [RT: %d][TL: %d]",numberOfCyclesToBeacon,(int)((ws->getParameters()->getSimTime()-ws->getTime())-(numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_EFF)));
	 ws->log->writeDebug(debug);
	 
	foundTime = numberOfCyclesToBeacon ;
	lastReturningTime = ws->getReturnTime();

	
	if (ws->getSensors()->inAnyGround())
	{
		puts("Got it! Returning to grid!");
		behaveState = GOTO_START;
		beingEfficient = true;
		beingVeryEfficient = true;
		foundTime=0;
	}

/*	if (haveExtraTime && !beingEfficient)
	{
		puts("I've got more time to explore :D");
		behaveState = GOTO_START;
	}
*/	
	if (!haveTime && !haveExtraTime)
	{
		puts("Ups, after all i've got no time. Returning to grid!");
		behaveState = GOTO_START;
	}

	return act;
}


Action * Meditator::actionGoToStart()
{
	static int dockstart = -1;


	decideBeaconToReturn();	

	Action *act;
	if ((act = dealWithCollision()) != NULL)
		return act;
	if (MathTools::distance( 0.0, 0.0, ws->getPosition()->getX(), ws->getPosition()->getY() ) < 1.0 )
		knowWellWayBetweenObjectives=true;

	if (ws->getTime()%50==25)
	{
		ws->setPlan(NULL);
		act = planner->wayTo(returnBeacon->getPosition());
		ws->setPlan(planner);
		foundTime = (int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF);
		lastReturningTime = ws->getReturnTime();
	}
	numberOfCyclesToBeacon = ws->getReturnTime()-lastReturningTime+foundTime; 
	
	if (!knowWellWayBetweenObjectives && ws->getTime()%100==0)
	{	
		planner->wayThroughKnownPlacesTo(returnBeacon->getPosition(),Position(0.0,0.0));
		estimatedCyclesEndKnownPlaces = planner->CyclesUntilObjective();

		planner->wayTo(returnBeacon->getPosition(),Position(0.0,0.0));
		estimatedCyclesEndUnKnownPlaces = planner->CyclesUntilObjective();
		
		double errorBetweenWays =
			((double) (abs(estimatedCyclesEndKnownPlaces-estimatedCyclesEndUnKnownPlaces)))/
			(estimatedCyclesEndKnownPlaces+estimatedCyclesEndUnKnownPlaces);

		if ( errorBetweenWays < 0.025)
			knowWellWayBetweenObjectives=true;
	}

	// tem tempo a + para ir até ao beacon e depois ir até ao ponto inicial?
	bool haveExtraTime = (ws->getParameters()->getSimTime()-ws->getTime()) >=
		(numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_EFF + TIME_TO_END_MARGIN);

	// tem tempo para ir até ao beacon e depois ir até ao ponto inicial mas não para explorar?
	bool haveTime = ((ws->getParameters()->getSimTime()-ws->getTime()) >=
		numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_EFF) && !haveExtraTime;


	if (haveExtraTime && !knowWellWayBetweenObjectives)
	{
		ws->setPlan(NULL);
		act = planner->wayTo(Position(0.0,0.0));
		
		beingEfficient=((ws->getReturnTime()+planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF)
				/estimatedCyclesEndKnownPlaces) < MOUSE_DISTACE_CYCLES_EFF;      
		beingVeryEfficient=((ws->getReturnTime()+planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_VEFF)
				/estimatedCyclesEndKnownPlaces)<MOUSE_DISTACE_CYCLES_VEFF;      

		ws->setPlan(planner);
		 
		char debug[255];
	    	sprintf(debug,"MEDITATOR - [EKP: %d][EUP: %d][RT: %d][TL: %d][CUOE: %d][CUOVE: %d][BE: %d][BVE: %d][KWW: %d]",estimatedCyclesEndKnownPlaces,estimatedCyclesEndUnKnownPlaces,numberOfCyclesToBeacon,(int)((ws->getParameters()->getSimTime()-ws->getTime()) -	(numberOfCyclesToBeacon+estimatedCyclesEndKnownPlaces*MOUSE_DISTACE_CYCLES_EFF + TIME_TO_END_MARGIN)),(int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF),(int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_VEFF),beingEfficient,beingVeryEfficient,knowWellWayBetweenObjectives);
	    	ws->log->writeDebug(debug);

		return act;
	}

	if (ws->getSensors()->inGround(returnBeacon->getNumber())) {
		beingEfficient=true;
		beingVeryEfficient=true;
	}

	if (!haveExtraTime && haveTime && !beingEfficient)
	{
		behaveState = GOTO_RETURN_BEACON;
		puts("No more exploration time, returning to beacon");
		return actionGoToReturnBeacon();
	}

	if (haveExtraTime && knowWellWayBetweenObjectives && !beingEfficient)
	{
		behaveState = GOTO_RETURN_BEACON;
		puts("Being slow, returning to beacon");
		return actionGoToReturnBeacon();
	}

	if (MathTools::distance( 0.0, 0.0, ws->getPosition()->getX(), ws->getPosition()->getY() ) < 0.05 ||
			ws->getTime()>=ws->getParameters()->getSimTime() ||
			(dockstart>=0 && ws->getTime() - dockstart > MAX_DOCK_TIME))
	{
		if (dockstart>=0 && ws->getTime() - dockstart > MAX_DOCK_TIME)
			puts("Fast dock! :|");
		ws->setPlan(NULL);
		return new Action(END,-ws->getMotors()->getInertiaLeft(),-ws->getMotors()->getInertiaRight());
	}
	else if (MathTools::distance( 0.0, 0.0, ws->getPosition()->getX(), ws->getPosition()->getY() ) < 1.0 )
	{
		if (dockstart < 0) dockstart = ws->getTime();
		ws->setPlan(NULL);
		if (ws->getSensors()->inCollision())
			return new Action(END,-ws->getMotors()->getInertiaLeft(),-ws->getMotors()->getInertiaRight());
		return motor->dockXY(0.0,0.0);
	}


	if (goingThroughKnownPlaces)
		act = planner->wayThroughKnownPlacesTo(Position(0.0,0.0));
	else act = planner->wayTo(Position(0.0,0.0));

	beingEfficient=((double)((double)ws->getReturnTime()+(double)planner->CyclesUntilObjective())/
			estimatedCyclesEndKnownPlaces)<MOUSE_DISTACE_CYCLES_EFF;
	beingVeryEfficient=((double)((double)ws->getReturnTime()+(double)planner->CyclesUntilObjective())/
			estimatedCyclesEndKnownPlaces)<MOUSE_DISTACE_CYCLES_VEFF;
	char debug[255];
	sprintf(debug,"MEDITATOR - [EKP: %d][EUP: %d][RT: %d][CUOE: %d][CUOVE: %d][BE: %d][BVE: %d][KWW: %d]",estimatedCyclesEndKnownPlaces,estimatedCyclesEndUnKnownPlaces,numberOfCyclesToBeacon,(int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF),(int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_VEFF),beingEfficient,beingVeryEfficient,knowWellWayBetweenObjectives);
	ws->log->writeDebug(debug);

	return act;
}

Action * Meditator::dealWithCollision()
{
	Action *act;

	if (ws->getSensors()->inCollision() &&
			ws->getSensors()->wasInCollision())
		toAbort=true;

	if (ws->getSensors()->CollidedRecently())
	{
		if (!toAbort)
		{
			act = new Action(-0.05,-0.05);
			return act;
		}
	}
	else toAbort = false;

	return NULL;
}

void Meditator::decideBeaconToReturn()
{
	static int beacon_grid_dist[25] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	static int min_bg_dist = 100000;

	if (returnBeacon==NULL) returnBeacon = ws->getBeacon(0);
	
	if (ws->getTime()%50 == 25 || ws->getTime()%50==0) return;
	
	for (int i=0; i<ws->getParameters()->getNBeacons(); i++)
		if (beacon_grid_dist[i]<0)
		{
			ws->setPlan(NULL);
			planner->wayThroughKnownPlacesTo(ws->getBeacon(i)->getPosition(),Position(0.0,0.0));
			ws->setPlan(planner);
			if (planner->ReachedEnd())
				beacon_grid_dist[i]=(int)(planner->CyclesUntilObjective()*MOUSE_DISTACE_CYCLES_EFF);
			break;
		}
	
	for (int i=0; i<ws->getParameters()->getNBeacons(); i++)
		if (beacon_grid_dist[i]>=0 && beacon_grid_dist[i]<min_bg_dist)
		{
			returnBeacon = ws->getBeacon(i);
			min_bg_dist = beacon_grid_dist[i];
		}
	if (ws->getTime()%50 == 12)
		beacon_grid_dist[rand()%ws->getParameters()->getNBeacons()]=-1;
	
	char debug[255];
	sprintf(debug,"RETURNING BEACON -");
	for (int i=0;i<ws->getParameters()->getNBeacons(); i++)
		sprintf(debug,"%s %d",debug,beacon_grid_dist[i]);
	sprintf(debug,"%s [%d,%d]",debug,returnBeacon->getNumber(),min_bg_dist);
	

	ws->log->writeDebug(debug);


}
