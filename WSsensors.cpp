#include "WSsensors.h"

WSsensors::WSsensors(WorldState *w)
{ 
	ws=w;
	leftdist=-1;
	centerleftdist=-1;
	centerrightdist=-1;
	rightdist=-1;
	compass=0.0;
	lastcollision=-100;
	compass_valid=false;
	lastcycle_compass_count=0;
}

void WSsensors::update()
{
	wasincollision = collision;
	collision = GetBumperSensor();
	if (collision) lastcollision = ws->getTime();

	groundReady = IsGroundSensorValid();
	if (hasGround())
		ground = GetGroundSensor();
	compass = GetCompassSensor();
	vLed=GetVisitingLed();

	leftsensor=(double)GetObstacleSensor(SENSOR_LEFT);
	centerleftsensor=(double)GetObstacleSensor(SENSOR_CENTERLEFT);
	centerrightsensor=(double)GetObstacleSensor(SENSOR_CENTERRIGHT);
	rightsensor=(double)GetObstacleSensor(SENSOR_RIGHT);

	leftsensorReady=IsObstacleSensorValid(SENSOR_LEFT);
	centerleftsensorReady=IsObstacleSensorValid(SENSOR_CENTERLEFT);
	centerrightsensorReady=IsObstacleSensorValid(SENSOR_CENTERRIGHT);
	rightsensorReady=IsObstacleSensorValid(SENSOR_RIGHT);

	if (leftsensor<=0.0) leftsensor=0.1;
	if (centerleftsensor<=0.0) centerleftsensor=0.1;
	if (centerrightsensor<=0.0) centerrightsensor=0.1;
	if (rightsensor<=0.0) rightsensor=0.1;

	leftdist=1/leftsensor;
	centerleftdist=1/centerleftsensor;
	centerrightdist=1/centerrightsensor;
	rightdist=1/rightsensor;

	compass_valid = IsCompassValid();

	char debug[255];
	sprintf(debug,"SENSORSREADY - left[%d] centerleft[%d] centerright[%d] right[%d] compass[%d]",
		hasLeftSensor(),hasCenterLeftSensor(),hasCenterRightSensor(),hasRightSensor(),compass_valid);
	if (ws->wasStopped()) {
		ws->log->writeDebug(debug);
	}

}

bool WSsensors::CollidedRecently(void)
{
	return ws->getTime()-lastcollision <= 3 ;
}

bool WSsensors::inAnyGround()
{
	for (int i=0; i<ws->getParameters()->getNBeacons(); i++)
		if (inGround(i))
			return true;
	return false;
}
