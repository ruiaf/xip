#include "WSparameters.h"

#include <stdio.h>

WSparameters::WSparameters()
{
	update();
}

void WSparameters::update()
{
	SimTime = GetFinalTime();
	NBeacons = GetNumberOfBeacons();
	//  RunningTimeout = GetRunningTimeout();
	CycleTime = GetCycleTime();

	CompassNoise = GetNoiseCompassSensor();
	BeaconNoise = GetNoiseBeaconSensor();
	ObstacleNoise = GetNoiseObstacleSensor();
	MotorsNoise = GetNoiseMotors();

	//  print();
}

void WSparameters::print(){

	printf("%d\n",SimTime);
	printf("%d\n",NBeacons);
	//  printf("%d\n",RunningTimeout);
	printf("%d\n",CycleTime);

	printf("%.2f\n",CompassNoise);
	printf("%.2f\n",BeaconNoise);
	printf("%.2f\n",ObstacleNoise);
	printf("%.2f\n",MotorsNoise);
}
