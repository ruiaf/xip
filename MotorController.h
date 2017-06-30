#ifndef _MOTOR_H_
#define _MOTOR_H_

class MotorController;

#include <iostream>

#include "Action.h"
#include "MathTools.h"
#include "Log.h"
#include "WorldState.h"
#include "LibComm.h"
#include "SensorRequest.h"
#include <list>

using namespace std;

class MotorController
{
 public:
  MotorController(WorldState *w,Log *l);

  void drive(Action *act,SensorRequest r);
  void requestSensors(SensorRequest r);
  Action * goToXY(double x_final,double y_final);
  Action * goSlowToXY(double x_final,double y_final);
  Action * dockXY(double x_final,double y_final);
  
  Action * goToXY(Position pos)
    { return goToXY(pos.getX(),pos.getY()); }
  Action * goSlowToXY(Position pos)
    { return goSlowToXY(pos.getX(),pos.getY()); }
  Action * dockXY(Position pos)
    { return dockXY(pos.getX(),pos.getY()); }
  
  bool wasActionCorrected() { return wasLastActionCorrected; } 
  bool isMouseStoppedAhead();
  void updateChangedActions();
  
 private:
  request_t transformRequest(SensorRequest sr);

  class ChangedAction
    {
    public:
      ChangedAction(int t, double xc,double yc): time(t), x(xc), y(yc) {}
      void print() {printf("time:%d x:%lf y:%lf\n",time,x,y);}
      int time;
      double x,y;
    };

  double hist_action[MAX_CYCLES][2];
  
  Log *log;
  WorldState *ws;
  
  bool wasLastActionCorrected;
  list<ChangedAction> listChangedActions;
};

#endif

