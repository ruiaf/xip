#ifndef _Action_H_
#define _Action_H_

class Action;

#include "MathTools.h"
#include <iostream>

using namespace std;

enum atype { MOTOR,
    RETURN,
    END,
    VISIT };

class Action {
public:
    Action(double l, double r);
    Action(enum atype, int i);
    Action(enum atype, double l, double r);

    void setType(enum atype a, int bv = -1);
    double getlPower(void) { return lPower; }
    double getrPower(void) { return rPower; }
    bool toVisit() { return actiontype == VISIT; }
    bool toEnd() { return actiontype == END; }
    bool toReturn() { return actiontype == RETURN; }
    bool toDrive() { return actiontype == MOTOR; }
    int getBeaconToVisit() { return beaconToVisit; }

private:
    enum atype actiontype;
    double lPower;
    double rPower;
    int beaconToVisit;
};

#endif
