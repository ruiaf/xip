#ifndef _Path_H_
#define _Path_H_

class Path;

#include "DiscretePosition.h"
#include "MathTools.h"
#include "Position.h"
#include <iostream>
#include <vector>

using namespace std;

class Path {
public:
    Path() { cost = totalcost = heuristic = 0; }

    void init(const DiscretePosition& curr);
    void clear(const DiscretePosition& dest);
    void clear() { cost = heuristic = totalcost = 0; }

    void update(Path* falher, double cost_from_last);
    double predictedCost(Path* falher, double cost_from_last);

    double getCost() { return totalcost; }
    double getValue() { return cost; }

    bool isFinal(void) { return current.distance(destination) < 2; }

    bool operator<(Path& p) { return getCost() < p.getCost(); }
    bool operator>(Path& p) { return getCost() > p.getCost(); }
    bool operator==(Path& p) { return getCost() == p.getCost(); }

    DiscretePosition getPosition() { return current; }
    bool isExpansioned() { return expansioned; }
    void setExpansioned() { expansioned = 1; }
    bool beenVisited() { return visited; }
    int getpqPlace() { return pqPlace; }
    void setpqPlace(int place) { pqPlace = place; }
    Path* getBefore() { return before; }

    void print()
    {
        cout << current.getX() << " " << current.getY() << ":";
        cout << " " << cost << " " << totalcost << "pq=" << pqPlace << " bef=" << before << " vis=" << visited << " exp=" << expansioned << endl;
        fflush(NULL);
    }

private:
    DiscretePosition current;
    static DiscretePosition destination;

    double cost;
    double heuristic;
    double totalcost;
    bool expansioned;
    int visited;
    int pqPlace;
    Path* before;
};
#endif
