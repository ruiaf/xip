#include "WorldState.h"

WorldState::WorldState(int pos, Log *l)
{
	time=0;
	mygridpos = pos;
	state=STOP;
	lastCycleState=STOP;
	actionSent=true;
	currentReturnTime=-1;
	log = NULL;
	
	// TODO: ACRESCENTAR OS SENSORES DOS OUTROS RATOS
	sensors = new WSsensors(this);
	motors = new WSmotors(this);
	// TODO: ACRESCENTAR AS POSIÇÕES DOS OUTROS RATOS
	position = new WSposition(this);
	maze = new WSmaze(this);
	beaconsearch = new WSbeaconsearch(this);
	plan = NULL;
}


WorldState::~WorldState()
{
	delete parameters;
	delete sensors;
	delete motors;
	delete position;
	for (int i=0; i<getParameters()->getNBeacons(); i++)
		delete beacon[i];
	//delete beacon;
	delete maze;
	delete beaconsearch;
	if (plan!=NULL)
		delete plan;
}

void WorldState::onlineWSCreation()
{
	parameters = new WSparameters();
	for (int i=0; i<getParameters()->getNBeacons(); i++)
		beacon.push_back(new WSbeacon(this,i));
	puts("I'm ready for rock and roll!!!");
}

void WorldState::setPlan(Plan * p)
{
	plan = p;
}

void WorldState::update(void)
{
	int pending_cycles, lost_cycles, oldtime;

	do {
		pending_cycles = ReadSensors ();
		if (pending_cycles < 0) {
			puts ("WorldState::update - Erro em ReadSensors!");
			exit (EXIT_FAILURE);
		}
		if (pending_cycles > 0)
			printf ("%d pending cycles :o\n", pending_cycles);

		oldtime=time;
		time=GetTime();

		lost_cycles = time - oldtime - 1;

		if (lost_cycles>0) {
			if (lost_cycles==1)
				puts ("Ups, caiu-me MESMO um ciclo!");
			else
				printf("Ups, caiu-me MESMO um ciclo... já é a %d vez esta semana!\n", lost_cycles);
		}

		lastCycleState=state;

		if (GetStartButton()) state=RUN;  // Start
		if (GetFinished())    state=FINISHED; // Finish
		if (GetStopButton())  state=STOP; // Interrupt

		// não alterar a ordem!

		sensors->update();
		if (!wasStopped()) motors->update();
		position->update();

		if (position->isLastCycleDirValid()) maze->update();

		if ( position->isLastCycleDirValid() && wasRunning()){
			for (int i=0; i<getParameters()->getNBeacons(); i++)
			{
				if(IsBeaconReady(i))
					beacon[i]->update(true);
				else beacon[i]->update(false);
			}
		}
		beaconsearch->update();

		if (wasRunning() && currentReturnTime>=0) currentReturnTime++;
		if (wasRunning() && sensors->inAnyGround()) currentReturnTime=0;

		char debug[255];
		sprintf(debug,"SENSORS - left[%.4lf] centerleft[%4lf] centerright[%4lf] right[%4lf]",getSensors()->getLeftSensor(),getSensors()->getCenterLeftSensor(),getSensors()->getCenterRightSensor(),getSensors()->getRightSensor());
		if (isRunning())
		  log->writeDebug(debug);
	} while (pending_cycles > 0);
}

bool WorldState::isAstarPosExp(double i, double j)
{
	if (!plan)
		return false;
	return plan->isAstarPosExp(DiscretePosition(Position(i,j)));	
}
