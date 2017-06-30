#ifndef _WSP_H_
#define _WSP_H_

class WSposition;

#include "Action.h"
#include "LibComm.h"
#include "MathTools.h"
#include "WorldState.h"
#include "defines.h"

using namespace std;

class WSposition {
public:
    WSposition(WorldState* w);

    void update();
    void PredictPosition(Action* act);

    Position getPos() { return Position(posX, posY); }
    Position getLagPos(int lag = LAG);
    double getDirRad(void) { return dirRad; }
    double getLagDirRad(int lag = LAG);
    double getDirDeg(void) { return dirDeg; }
    double getLagDirDeg(int lag = LAG);
    double getX(void) { return posX; }
    double getY(void) { return posY; }

    double getLastDirRad(void) { return lastDirRad; }
    double getLastDirDeg(void) { return lastDirDeg; }
    double getLastX(void) { return lastPosX; }
    double getLastY(void) { return lastPosY; }
    bool isLastCycleDirValid(void) { return lastCycleDirValid; }

    double getThisCyclePredictedDirRad(void) { return thisCyclePredictedDirRad; }
    double getThisCyclePredictedDirDeg(void) { return thisCyclePredictedDirDeg; }
    double getThisCyclePredictedX(void) { return thisCyclePredictedPosX; }
    double getThisCyclePredictedY(void) { return thisCyclePredictedPosY; }

    bool nearEnd(void) { return hypot(posX, posY) < 1; }

    void adjustByMaze(double incx, double incy);

    void loadWorldState();

    double getMinXpossible() { return minXpossible; }
    double getMaxXpossible() { return maxXpossible; }
    double getMinYpossible() { return minYpossible; }
    double getMaxYpossible() { return maxYpossible; }

private:
    void updateMinMaxEver(void);

    double dirRad;
    double dirDeg;
    double posX;
    double posY;

    double hist_pos[MAX_CYCLES][4];

    bool lastCycleDirValid;
    double suposedDirRad;
    double suposedDirDeg;

    double lastDirRad;
    double lastDirDeg;
    double lastPosX;
    double lastPosY;

    bool havePerfect;
    double pXini, pYini;

    double minXever;
    double maxXever;
    double minYever;
    double maxYever;

    double minXpossible;
    double maxXpossible;
    double minYpossible;
    double maxYpossible;

    double thisCyclePredictedDirRad;
    double thisCyclePredictedDirDeg;
    double thisCyclePredictedPosX;
    double thisCyclePredictedPosY;

    WorldState* ws;
};

#endif
