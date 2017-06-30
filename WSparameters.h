#ifndef _WSPARAM_H_
#define _WSPARAM_H_

class WSparameters;

#include "LibComm.h"

class WSparameters
{
 public:

  WSparameters();

  void update();

  int getSimTime() { return SimTime; }
  int getNBeacons() { return NBeacons; }
  int getCycleTime() { return CycleTime; }

  double getCompassNoise() { return CompassNoise; }
  double getBeaconNoise() { return BeaconNoise; }
  double getObstacleNoise() { return ObstacleNoise; }
  double getMotorsNoise() { return MotorsNoise; }

 private:

  void print();

  int SimTime;
  int NBeacons;
  int CycleTime;

  double CompassNoise;
  double BeaconNoise;
  double ObstacleNoise;
  double MotorsNoise;

};

#endif

