#include "WSmotors.h"

WSmotors::WSmotors(WorldState *w)
{
  ws=w;
  
  seclastActionTime = -2;
  seclastPowerLeft = 0.0;
  seclastPowerRight = 0.0;

  lastActionTime=-1;
  lastPowerRight=0.0;
  lastPowerLeft=0.0;

  inertiaLeft=0.0;
  inertiaRight=0.0;
}

void WSmotors::action_done(Action * act)
{
  
  seclastActionTime = lastActionTime;
  seclastPowerLeft = lastPowerLeft;
  seclastPowerRight = lastPowerRight;
  if (ws->ActionSent())
    {
      lastActionTime = ws->getTime();
      lastPowerLeft  = act->getlPower();
      lastPowerRight = act->getrPower();
    }
}

double WSmotors::getInertiaLeft(int lag) { return hist_inertia[ws->getTime()-lag][0]; }

double WSmotors::getInertiaRight(int lag) { return hist_inertia[ws->getTime()-lag][1]; }

void WSmotors::update()
{
  if (ws->wasStopped()) return;
  if (ws->getSensors()->inCollision())
    {
      //inertiaLeft = 0; // sera 0?
      //inertiaRight = 0; // sera 0?
    }
  else
    {
      inertiaLeft = (inertiaLeft+lastPowerLeft)/2;
      inertiaRight = (inertiaRight+lastPowerRight)/2;
      hist_inertia[ws->getTime()][0] = inertiaLeft;
      hist_inertia[ws->getTime()][1] = inertiaRight;
    }
}

double WSmotors::getMaxPowerLeftTurn(double lastLPower, double lastRPower, double *finalLInertia, double *finalRInertia)
{
	*finalLInertia = (lastLPower - 0.15) / 2.0;
	*finalRInertia = (lastRPower + 0.15) / 2.0;
	return (*finalRInertia - *finalLInertia) / 1.0 ;
}

double WSmotors::getMaxPowerFront(double lastLPower, double lastRPower, double *finalLInertia, double *finalRInertia)
{
	*finalLInertia = (lastLPower + 0.15) / 2.0;
	*finalRInertia = (lastRPower + 0.15) / 2.0;
	return (*finalLInertia + *finalRInertia) / 2.0 ;
}

