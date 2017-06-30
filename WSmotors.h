#ifndef _WSM_H_
#define _WSM_H_

class WSmotors;

#include "Action.h"
#include "LibComm.h"
#include "MathTools.h"
#include "WorldState.h"
#include "defines.h"

class WSmotors {
public:
    WSmotors(WorldState* w);

    void update();
    void action_done(Action* act);

    double getLastPowerLeft() { return lastPowerLeft; }
    double getLastPowerRight() { return lastPowerRight; }

    double getInertiaLeft(int lag = 0);
    double getInertiaRight(int lag = 0);

    double predictInertiaLeft(double lp) { return (inertiaLeft + lp) / 2; }
    double predictInertiaRight(double rp) { return (inertiaRight + rp) / 2; }

    double getPredictedPower(double lastPower, double inPower) { return (lastPower * inPower) / 2.0; }
    double getMaxPowerFront(double lastLPower, double lastRPower, double* finalLInertia, double* finalRInertia);
    double getMaxPowerLeftTurn(double lastLPower, double lastRPower, double* finalLInertia, double* finalRInertia);
    double getMaxPowerRightTurn(double lastLPower, double lastRPower, double* finalLInertia, double* finalRInertia)
    {
        return getMaxPowerLeftTurn(lastRPower, lastLPower, finalRInertia, finalLInertia);
    }

private:
    int lastActionTime;
    double lastPowerRight;
    double lastPowerLeft;

    int seclastActionTime;
    double seclastPowerRight;
    double seclastPowerLeft;

    double inertiaLeft;
    double inertiaRight;

    double hist_inertia[MAX_CYCLES][2];

    WorldState* ws;
};

#endif
