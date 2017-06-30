#ifndef _POS_H_
#define _POS_H_

class Position;

#include "MathTools.h"
#include <stdio.h>
class Position
{
public:
  Position(double x=0.0, double y=0.0) { PosX=x; PosY=y; }

  double getX() const { return PosX; }
  double getY() const { return PosY; }
  void setX(double x) { PosX=x; }
  void setY(double y) { PosY=y; }
  double distance(const Position &other) const
    {
      return MathTools::distance(getX(),getY(),other.getX(),other.getY());
    }
  void print() { printf("(Pos x:%lf y:%lf)\n",PosX,PosY); }
  
private:
  double PosX;
  double PosY;
};

#endif
