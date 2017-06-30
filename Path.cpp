#include "Path.h"

// c++ is archaich, static has to be declared outside
DiscretePosition Path::destination;

void Path::init(const DiscretePosition& curr)
{
    expansioned = 0;
    visited = 0;
    pqPlace = -1;
    cost = totalcost = heuristic = 0;
    current = curr;
    before = NULL;
}

void Path::clear(const DiscretePosition& dest)
{
    expansioned = 0;
    visited = 0;
    pqPlace = -1;
    destination = dest;
    before = NULL;
}

void Path::update(Path* father, double cost_from_last)
{
    visited = 1;
    cost = father->getValue() + cost_from_last;
    heuristic = current.distance(destination);
    totalcost = cost + heuristic;
    before = father;
}

double Path::predictedCost(Path* father, double cost_from_last)
{
    return father->getValue() + cost_from_last + current.distance(destination);
}
