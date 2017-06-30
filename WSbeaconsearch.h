#ifndef _BECONSEARCH_H_
#define _BECONSEARCH_H_

class WSbeaconsearch;

#include "MathTools.h"
#include "Position.h"
#include "WorldState.h"
#include "defines.h"
#include <vector>

#define AREA_SIZE 2
#define AREA_WIDTH (2 * LAB_X / AREA_SIZE)
#define AREA_HEIGHT (2 * LAB_Y / AREA_SIZE)
#define N_AREAS (AREA_WIDTH * AREA_HEIGHT)

using namespace std;

enum areastate { NO_INFO = 0,
    LOW_PROB_SEEN,
    MID_PROB_SEEN,
    HIGH_PROB_SEEN,
    VISITED,
    UNREACHABLE };

class WSbeaconsearch {
public:
    WSbeaconsearch(WorldState* w);
    Position getProbableBeaconPos();
    void update();
    void markCurrentPositionAsUnreachable();
    bool allUnreachableOrVisited();

private:
    class SearchArea {
    public:
        bool operator<(const SearchArea& sa) const;

        double x, y;
        enum areastate s;
        WorldState* ws;
    };

    int min;
    bool outOfBounds(int i);
    bool inArea(int i);

    WorldState* ws;
    vector<SearchArea> search_area;
};

#endif
