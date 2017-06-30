#ifndef _WSB_H_
#define _WSB_H_

class WSbeacon;

#include "MathTools.h"
#include "Action.h"
#include "defines.h"
#include "WorldState.h"

using namespace std;

#define BEACON_DEF 10

class WSbeacon
{

public:
	WSbeacon(WorldState *w, int bindex);

	void update(bool isBeaconReady);
	
	double getDirRad(void) { return -1; }
	double getDegDir(void) { return -1; }
	
	bool inBeaconRadius(double x, double y)
	  { 
	    if (MathTools::distance(x,y,getX(),getY()) < (RAIO_FAROL-DIAMETER/2))
	      return true;
	    return false;
	  }
	
	Position getPosition();
	double getX(); // deprecated
	double getY(); // deprecated

	int getProb() {
		if (best_point_value>90)
			return 4;
		else if (best_point_value>50)
			return 3;
		else if (best_point_value>20)
			return 2;
		else if (best_point_value>0)
			return 1;
		return 0;
	}

	bool onSight(void) { return measure.beaconVisible; }
	char getVal(double x, double y);
	void markCurrentPositionAsUnreachable();
	int getNumber() { return beacon_index; }
	bool hasBeenVisited() { return beenSetVisited; }
	void setVisited() { beenSetVisited=true; }
	void setUnvisited() { beenSetVisited=false; }

	
private:

	beaconMeasure measure;
	WorldState *ws;

	int beacon_index;
	bool beenSetVisited;

	void applySensor(double dir);
	void clearMouse();
	void setBeaconMatPos(int xmazepos, int ymazepos, char val);
	void findBest();
	void clearArroundBeacon();

	/* funções de probabilidade */
	void init_prob(void);
	double distri_normal_acc(double dif,double dif_max);
	double prob_acc[200];
	void createSensorMatrix();
	
	int beacon_prob[2*LAB_X*BEACON_DEF][2*LAB_Y*BEACON_DEF];
	int samples[2*LAB_X*BEACON_DEF][2*LAB_Y*BEACON_DEF];

	char sensor_prob[361][2*LAB_X*BEACON_DEF][2*LAB_Y*BEACON_DEF];

	DiscretePosition best_point;
	double best_point_value;
	bool know_point;
};

#endif
