#include "WSposition.h"

WSposition::WSposition(WorldState *w)
{
	ws=w;

	havePerfect=false; // TODO: MUDAR ISTO PARA TRUE??? E CORRIGIR A ACTUALIZACAO DA POS
	posX=0.0;
	posY=0.0;
	dirRad=0.0;
	dirDeg=0.0;
	lastCycleDirValid=false;
	suposedDirRad=0.0;
	suposedDirDeg=0.0;

	lastPosX=0.0;
	lastPosY=0.0;
	lastDirRad=0.0;
	lastDirDeg=0.0;

	minXpossible=-LAB_X;
	maxXpossible=LAB_X;
	minYpossible=-LAB_Y;
	maxYpossible=LAB_Y; 

	minXever=posX;
	maxXever=posX;
	minYever=posY;
	maxYever=posY;
}


void WSposition::update()
{ 
	static int n_amostras_no_ciclo;

	// adjust position based on 

	lastPosX=posX;
	lastPosY=posY;
	lastDirRad=dirRad;
	lastDirDeg=dirDeg;

	if (havePerfect) // com -gps
	{
		if (!lastCycleDirValid) // TODO: AGORA GPS TEM ERRO. MUDAR PARA WHILE CICLO=0 E FAZER MEDIA
								//		 ACRESCENTAR PARA A MEDIA.
		{
			lastCycleDirValid =true;
			pXini = GetX();
			pYini = GetY();
		}
		posX   = GetX()-pXini; // TODO: CRIAR UMA REAL POS para depois usar na comunicação
		posY   = GetY()-pYini;
		dirDeg = GetDir();
		dirRad = MathTools::deg2rad(dirDeg);

		updateMinMaxEver(); // TODO: Isto já n é necessário. Com o gps sabemos exactamento onde é o MinMAx ever
		return;
	}


	if (!lastCycleDirValid && ws->getSensors()->hasValidCompass())
	{
		double comp;
		comp = ws->getSensors()->getCompass();

		dirDeg = MathTools::normalize_180_to_180(comp);
		dirRad = MathTools::deg2rad(dirDeg);
		lastCycleDirValid=true;
		n_amostras_no_ciclo = 1;
	}
	else
	{
		// ajustar a posição à LAG ciclos atras baseada na bussula com lag
		if (ws->getSensors()->hasValidCompass()) {

			double mycomp = MathTools::deg2rad(ws->getSensors()->getCompass());
			int lagtime=((ws->getTime()-LAG)>0)?(ws->getTime()-LAG):(0);

			//double dirRad;
			double dirdif = MathTools::normalize(hist_pos[lagtime][0] - atan2(sin(mycomp)+sin(hist_pos[lagtime][0]),cos(mycomp)+cos(hist_pos[lagtime][0])));
			hist_pos[lagtime][0] = MathTools::normalize(hist_pos[lagtime][0] - dirdif);

			//double dirDeg;
			hist_pos[lagtime][1] = MathTools::rad2deg(hist_pos[lagtime][0]);

			// corrigir o impacto do erro nos ciclos seguintes

			for (int i = (ws->getTime()-lagtime)-1;i>0;i--) {
				lagtime = ws->getTime()-i;

				double difposx = ((ws->getMotors()->getInertiaLeft(i)+ws->getMotors()->getInertiaRight(i))*cos(hist_pos[lagtime-1][0]))/2;
				double difposy = ((ws->getMotors()->getInertiaLeft(i)+ws->getMotors()->getInertiaRight(i))*sin(hist_pos[lagtime-1][0]))/2;

				hist_pos[lagtime][0] = MathTools::normalize(hist_pos[lagtime][0] - dirdif);
				hist_pos[lagtime][1] = MathTools::rad2deg(hist_pos[lagtime][0]);
				hist_pos[lagtime][2] = hist_pos[lagtime-1][2]+difposx;
				hist_pos[lagtime][3] = hist_pos[lagtime-1][3]+difposy;
			}

			if (!ws->wasStopped()) {
				lastDirRad = hist_pos[max(ws->getTime()-1,0)][0];
				lastDirDeg = hist_pos[max(ws->getTime()-1,0)][1];
				lastPosX = hist_pos[max(ws->getTime()-1,0)][2];
				lastPosY = hist_pos[max(ws->getTime()-1,0)][3];
			}

			char debug[256];
			sprintf(debug,"COMPASS ADJUST - (%lf,%lf,%lf,%lf) - INPUT COMPASS (%lf) - CALC DIF (%lf)",
					hist_pos[ws->getTime()-1][0],hist_pos[ws->getTime()-1][1],hist_pos[ws->getTime()-1][2],hist_pos[ws->getTime()-1][3],
					mycomp,dirdif);

			if (!ws->wasStopped() || rand()%100==0)
				ws->log->writeDebug(debug);
		}

		if (!ws->wasStopped()) {
			// end
			n_amostras_no_ciclo = 1;
			dirRad = MathTools::normalize( lastDirRad + (ws->getMotors()->getInertiaRight() - ws->getMotors()->getInertiaLeft())/DIAMETER );
			dirDeg = MathTools::rad2deg(dirRad);
		}
		//
		//double comp;

		//comp = MathTools::deg2rad(ws->getSensors()->getCompass());

		// verificar se isto faz a média direito!! (acho q sim...)
		//dirRad = MathTools::normalize(atan2(sin(comp)+sin(suposedDirRad),cos(comp)+cos(suposedDirRad)));
		//dirRad = MathTools::normalize(atan2(sin(suposedDirRad),cos(suposedDirRad)));
		//dirDeg = MathTools::rad2deg(dirRad);
	}

	if (!ws->wasStopped() && !ws->getSensors()->inCollision()) {
		posX = lastPosX + ((ws->getMotors()->getInertiaLeft()+ws->getMotors()->getInertiaRight())*cos(lastDirRad))/2;
		posY = lastPosY + ((ws->getMotors()->getInertiaLeft()+ws->getMotors()->getInertiaRight())*sin(lastDirRad))/2;
	}

	hist_pos[ws->getTime()][0] = dirRad;
	hist_pos[ws->getTime()][1] = dirDeg;
	hist_pos[ws->getTime()][2] = posX;
	hist_pos[ws->getTime()][3] = posY;

	char debug[1000];
	sprintf(debug,"Postition - (%lf,%lf,%lf,%lf) (%lf,%lf,%lf,%lf) (%lf,%lf,%lf,%lf) (%lf,%lf,%lf,%lf) (%lf,%lf,%lf,%lf)",
			hist_pos[ws->getTime()][0],hist_pos[ws->getTime()][1],hist_pos[ws->getTime()][2],hist_pos[ws->getTime()][3],
			hist_pos[ws->getTime()-1][0],hist_pos[ws->getTime()-1][1],hist_pos[ws->getTime()-1][2],hist_pos[ws->getTime()-1][3],
			hist_pos[ws->getTime()-2][0],hist_pos[ws->getTime()-2][1],hist_pos[ws->getTime()-2][2],hist_pos[ws->getTime()-2][3],
			hist_pos[ws->getTime()-3][0],hist_pos[ws->getTime()-3][1],hist_pos[ws->getTime()-3][2],hist_pos[ws->getTime()-3][3],
			hist_pos[ws->getTime()-4][0],hist_pos[ws->getTime()-4][1],hist_pos[ws->getTime()-4][2],hist_pos[ws->getTime()-4][3]);

	if (!ws->wasStopped())
		ws->log->writeDebug(debug);

	updateMinMaxEver();
}


Position WSposition::getLagPos(int lag) { return Position(hist_pos[ws->getTime()-lag][2],hist_pos[ws->getTime()-lag][3]); }
double WSposition::getLagDirRad(int lag) { return hist_pos[max(ws->getTime()-lag,0)][0]; }
double WSposition::getLagDirDeg(int lag) { return hist_pos[max(ws->getTime()-lag,0)][1]; }


void WSposition::adjustByMaze(double incx, double incy)
{
	posX+=incx;
	posY+=incy; 
}

void WSposition::loadWorldState()
{
	havePerfect=true;
}

void WSposition::updateMinMaxEver()
{
	minXever = min(minXever,posX);
	minYever = min(minYever,posY);
	maxXever = max(maxXever,posX);
	maxYever = max(maxYever,posY);

	minXpossible = maxXever-LAB_X;
	maxXpossible = minXever+LAB_X;
	minYpossible = maxYever-LAB_Y;
	maxYpossible = minYever+LAB_Y;  
}

// para no caso de colidir saber onde é que estava a pensar ir
void WSposition::PredictPosition(Action *act)
{
	thisCyclePredictedDirRad = MathTools::normalize( dirRad + 
			(ws->getMotors()->predictInertiaRight(act->getrPower()) -
			 ws->getMotors()->predictInertiaLeft(act->getlPower()))/DIAMETER );
	thisCyclePredictedDirDeg = MathTools::rad2deg(thisCyclePredictedDirRad);

	thisCyclePredictedPosX = posX + ((ws->getMotors()->predictInertiaRight(act->getrPower())
				+ws->getMotors()->predictInertiaLeft(act->getlPower()))*cos(dirRad))/2;
	thisCyclePredictedPosY = posY + ((ws->getMotors()->predictInertiaRight(act->getrPower())
				+ws->getMotors()->predictInertiaLeft(act->getlPower()))*sin(dirRad))/2;
}
