#ifndef _WSS_H_
#define _WSS_H_

class WSsensors;

#include "LibComm.h"
#include "defines.h"
#include "MathTools.h"
#include "Action.h"
#include "WorldState.h"

using namespace std;

class WSsensors
{
 public:
  WSsensors(WorldState *w);

  void update(void);
  void predict(Action *act);

  /**/
  double getLeftDist(void) { return leftdist; }
  double getCenterLeftDist(void) { return centerleftdist; }
  double getCenterRightDist(void) { return centerrightdist; }
  double getRightDist(void) { return rightdist; }

  double getLeftSensor(void) { return leftsensor; }
  double getCenterLeftSensor(void) { return centerleftsensor; }
  double getCenterRightSensor(void) { return centerrightsensor; }
  double getRightSensor(void) { return rightsensor; }

  bool hasLeftSensor(void) { return leftsensorReady; }
  bool hasCenterLeftSensor(void) { return centerleftsensorReady; }
  bool hasCenterRightSensor(void) { return centerrightsensorReady; }
  bool hasRightSensor(void) { return rightsensorReady; }
  bool hasIRSensors(void) { return rightsensorReady && leftsensorReady && centerleftsensorReady && centerrightsensorReady; }

  double getCompass(void) { return compass;}
  bool hasValidCompass(void) { return compass_valid; }
  double CompassIsFromThisCycle(void);

  bool hasHighRisk() { return leftdist<=0.5 || centerleftdist<=0.5 || rightdist<=0.5 || centerrightdist<=0.5;}
  bool inAnyGround();
  bool visitingLed() { return vLed; }
  bool inGround(int index) { return ground==index; }
  bool hasGround() { return groundReady; }
  bool inCollision(void) { return collision; }
  bool wasInCollision(void) { return wasincollision; }
  bool CollidedRecently(void);
  //bool isFinished(void) { return finish; } n funciona?

 private:
  int ground;
  bool groundReady;

  bool collision;
  bool wasincollision;
  int lastcollision;
  bool vLed;
  //bool finish;
  
  double compass;
  bool compass_valid;

  double leftsensor;
  double centerleftsensor;
  double centerrightsensor;
  double rightsensor;

  bool leftsensorReady;
  bool centerleftsensorReady;
  bool centerrightsensorReady;
  bool rightsensorReady;

  double leftdist;
  double centerleftdist;
  double centerrightdist;
  double rightdist;

  WorldState *ws;

  int lastcycle_compass_count;

};

#endif
