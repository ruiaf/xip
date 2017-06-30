#ifndef _WorldState_H_
#define _WorldState_H_

class WorldState;

#include <stdio.h>
#include <sys/timeb.h>
#include "LibComm.h"
#include "MathTools.h"
#include "Action.h"
#include "defines.h"
#include "Position.h"

#include "Plan.h"
#include "WSsensors.h"
#include "WSmotors.h"
#include "WSposition.h"
#include "WSmaze.h"
#include "WSbeacon.h"
#include "WSparameters.h"
#include "Path.h"
#include "WSbeaconsearch.h"
#include "Log.h"
#include <vector>

using namespace std;

class WorldState
{
	public:

		WorldState(int pos, Log *l);
		~WorldState();

		void update();
		int getTime(void) { return time; }
		int getReturnTime(void) {return currentReturnTime;}
		int getMyGridPos(void) { return mygridpos; }

		bool isRunning(void) { return state==RUN; }
		bool isFinished(void) { return state==FINISHED; }
		bool isStopped(void) { return state==STOP; }

		bool wasRunning(void) { return lastCycleState==RUN; }
		bool wasFinished(void) { return lastCycleState==FINISHED; }
		bool wasStopped(void) { return lastCycleState==STOP; }
		void setActionSent(bool b) { actionSent=b; }
		bool ActionSent() { return actionSent; }
		WSparameters * getParameters(void) { return parameters; }
		WSsensors * getSensors(void) { return sensors; }
		WSmotors * getMotors(void) { return motors; }
		WSposition * getPosition(void) { return position; }
		WSmaze * getMaze(void) { return maze; }
		WSbeacon * getBeacon(int i) { return beacon[i]; }
		WSbeaconsearch * getBeaconsearch(void) { return beaconsearch; }
		Plan * getPlan(void) { return plan; }
		bool isAstarPosExp(double i, double j);
		
		void onlineWSCreation();
		void loadWorldState(char * WSFileName);
		void setPlan(Plan * p);
		void setLog(Log *l) { log = l; }

		Log *log;
	private:

		int time;
		int state;
		int lastCycleState;
		int mygridpos;
		bool actionSent;
				
		int currentReturnTime;

		WSsensors * sensors;
		WSmotors * motors;
		WSposition * position;
		WSmaze * maze;
		
		int nBeacons;
		vector<WSbeacon *> beacon;
		WSbeaconsearch *beaconsearch;
		WSparameters * parameters;

		Plan * plan;

};

#endif
