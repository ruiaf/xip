#include "WSbeaconsearch.h"

WSbeaconsearch::WSbeaconsearch(WorldState* w)
{
    SearchArea sa;
    ws = w;
    for (int i = 0; i < AREA_WIDTH; i++)
        for (int j = 0; j < AREA_HEIGHT; j++) {
            sa.x = i * AREA_SIZE - LAB_X;
            sa.y = j * AREA_SIZE - LAB_Y;
            sa.s = NO_INFO;
            sa.ws = w;
            search_area.push_back(sa);
        }
}

Position WSbeaconsearch::getProbableBeaconPos()
{
    return Position(search_area[min].x, search_area[min].y);
}

void WSbeaconsearch::update()
{
    min = 0;

    for (unsigned i = 0; i < search_area.size(); i++) {
        if (search_area[i].s == UNREACHABLE)
            continue;

        if (outOfBounds(i)) {
            search_area[i].s = UNREACHABLE;
            continue;
        }

        if (inArea(i)) {
            search_area[i].s = VISITED;
            continue;
        }

        if (search_area[i].s >= HIGH_PROB_SEEN)
            continue;

        enum areastate new_s;
        double dist = MathTools::distance(ws->getPosition()->getX(),
            ws->getPosition()->getY(),
            (search_area[i].x + AREA_SIZE),
            (search_area[i].y + AREA_SIZE));
        if (dist < 4)
            new_s = HIGH_PROB_SEEN;
        else if (dist < 8)
            new_s = MID_PROB_SEEN;
        else
            new_s = LOW_PROB_SEEN;

        DiscretePosition mypos = DiscretePosition(Position(search_area[i].x, search_area[i].y));
        if (new_s > search_area[i].s && ws->getMaze()->goodToGoWithMouse(mypos.getX(), mypos.getY()) < 3)
            search_area[i].s = new_s;
    }

    for (int i = search_area.size() - 1; i >= 0; i--) {
        if (search_area[i] < search_area[min])
            min = i;
    }

    return;
}

void WSbeaconsearch::markCurrentPositionAsUnreachable()
{
    search_area[min].s = UNREACHABLE;
}

bool WSbeaconsearch::outOfBounds(int i)
{
    if (ws->getMaze()->inBounds(search_area[i].x + AREA_SIZE / 2,
            search_area[i].y + AREA_SIZE / 2))
        return false;
    return true;
}

bool WSbeaconsearch::allUnreachableOrVisited()
{
    for (int i = search_area.size() - 1; i >= 0; i--) {
        if (search_area[i].s != UNREACHABLE && search_area[i].s != VISITED)
            return false;
    }
    puts("Todos os pontos tapados. A recomeçar a detecção de paredes");
    for (int i = search_area.size() - 1; i >= 0; i--) {
        if (search_area[i].s == UNREACHABLE)
            search_area[i].s = LOW_PROB_SEEN;
    }
    return true;
}

bool WSbeaconsearch::inArea(int i)
{
    if (ws->getPosition()->getX() >= search_area[i].x - AREA_SIZE && ws->getPosition()->getX() <= search_area[i].x + 2 * AREA_SIZE && ws->getPosition()->getY() >= search_area[i].y - AREA_SIZE && ws->getPosition()->getY() <= search_area[i].y + 2 * AREA_SIZE)
        return true;

    return false;
}

bool WSbeaconsearch::SearchArea::operator<(const SearchArea& sa) const
{
    if ((this->s) != (sa.s))
        return (this->s) < (sa.s);

    return MathTools::distance(ws->getPosition()->getX(), ws->getPosition()->getY(),
               (this->x + AREA_SIZE),
               (this->y + AREA_SIZE))
        < MathTools::distance(ws->getPosition()->getX(), ws->getPosition()->getY(),
              (sa.x + AREA_SIZE),
              (sa.y + AREA_SIZE));
}
