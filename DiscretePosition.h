#ifndef _DISC_POS_H_
#define _DISC_POS_H_

class DiscretePosition;

#include "defines.h"
#include "Position.h"

class DiscretePosition
{
 public:
  
  DiscretePosition()
    {
      x = 0;
      y = 0;
    }
  
  DiscretePosition(int i, int j)
    {
      x = i;
      y = j;
    }

  DiscretePosition(Position pos)
    {
      x = (int) round((pos.getX()+LAB_X)*MAZE_DEF);
      y = (int) round((pos.getY()+LAB_Y)*MAZE_DEF);
    }

  double distance(const DiscretePosition &other) const
    {
      return MathTools::distance(getX(),getY(),other.getX(),other.getY());
    }

  void incX() { x++; }
  void incY() { y++; }
  int getX() const { return x; }
  int getY() const { return y; }
  void setX(int xx) { x=xx; }
  void setY(int yy) { y=yy; }

  Position getPosition() { return Position(((double)x/MAZE_DEF)-LAB_X,
					   ((double)y/MAZE_DEF)-LAB_Y); }

  bool operator==(DiscretePosition & p) const { return ((getX() == p.getX()) &&
							(getY() == p.getY())); }
  bool operator!=(DiscretePosition & p) const { return !(getX() == p.getX() &&
							 getY() == p.getY()); }
  
 private:
  int x;
  int y;
};

#endif
