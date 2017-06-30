#ifndef _WSMZ_H_
#define _WSMZ_H_

class WSmaze;

#include "LibComm.h"
#include "MathTools.h"
#include "Action.h"
#include "defines.h"
#include "WorldState.h"
#include "Position.h"
#include "WSposition.h"
#include "MathTools.h"
#include "assert.h"

#define MIN_SENSOR_VALID 1.5
#define PROB_CONSID_WALL 30
#define MIN_TO_BE_KNOWN 30
#define GOOD_MOUSE_MARGIN 8
#define GO_MOUSE_MARGIN 4
#define KNOWN_MARGIN 5
#define VALID_GO_MARGIN 5
#define POS_MOVE_MARGIN 0.05
#define SMALL_LAB_X 4
#define SMALL_LAB_Y 4
#define ACTION_RADIUS 5

#define DO_ERODE_DILATE
//#define DO_SMALL_WS

using namespace std;

typedef struct {
  int size;
  char mat[MAZE_DEF*12][MAZE_DEF*12];
} matrix;  

class WSmaze
{
 public:
  
  WSmaze(WorldState *w);
  void update();
  char getMazePos(double x, double y);
  
  bool inBounds(double x,double y);
  bool inMatrixBounds(int x,int y);
  
  char getSmallMazePos(double x, double y);
  bool inSmallMazeBounds(double x,double y);
  bool inSmallMazeMatrixBounds(int x,int y);
  
  bool validToGoWithMouse(int x, int y);
  bool validToGoWithMouse(const DiscretePosition & pos) { return validToGoWithMouse(pos.getX(),pos.getY()); }
  void loadWorldState(double gridx,double gridy,char mazeMap[140][285]);
  double goodToGoWithMouse(int x, int y);
  double goodToOptimizeWithMouse(int x, int y);
  double goodToGoWithMouse(const DiscretePosition & pos) { return goodToGoWithMouse(pos.getX(),pos.getY());}
  double goodToOptimizeWithMouse(const DiscretePosition & pos) { return goodToOptimizeWithMouse(pos.getX(),pos.getY());}

  bool CanGoWithMouse(int x, int y);
  bool CanGoWithMouse(const DiscretePosition & pos) { return CanGoWithMouse(pos.getX(),pos.getY()); }
  bool isKnown(int x, int y);
  bool isKnown(const DiscretePosition & pos) { return isKnown(pos.getX(),pos.getY()); }
  double nearWall(int x, int y);
  void erodeMazeToUnknown();
  void PaintMouseStoppedAhead();

  void setToBounds(Position &pos);
  
 private:
  
  void applyAllSensors();
  void applySensor(double sensorVal, double sensorDist, double angle);
  void applyCleanSensor(double sensorVal, double sensorDist, double angle);
  void applyRobotMatrix(double X, double Y);
  void applyWallRobotMatrix(double X, double Y);
  void applyWallRobotMatrixAfterProcess(double X, double Y);
  void createRobotMatrix(void);

  void applySensorMatrix(int ang,double sensordist,double sensor_val,double X, double Y,double conf);
  void applyCleanSensorMatrix(int ang,double sensordist,double sensor_val,double X, double Y,double conf);
  void setMazePos(double x, double y, char val,double certeza);
  void setMazeMatPos(int xmazepos, int ymazepos, char val,double certeza);
  void setMazeMatPosAfterProcess(int xmazepos, int ymazepos, char val,double certeza);

  void createSensor(void);
  void createGoodMouseMatrix(void);
  void createGoMouseMatrix(void);
  void createKnownMatrix(void);
  void createValidGoMatrix(void);

  void clearGoodToGoMouseMatrix();
  void clearIsKnownMatrix();
  void clearCanGoWithMouseMatrix();

  void doErodesAndDilates();
  void adjustByMazesDifferences();  
  void erode(char newmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y], char oldmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y]);
  void dilate(char newmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y], char oldmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y]);
    void erodeToUnknown(char newmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y], char oldmatrix[2*MAZE_DEF*LAB_X][2*MAZE_DEF*LAB_Y]);


  /* funções de probabilidade */
  void init_prob(void);
  double distri_normal_acc(double dif,double dif_max);
  
  
  /********************************************************/
  /* SMALL WORLD STATE funcs                              */
  /********************************************************/
  
  void setSmallMazePos(double x, double y, char val,double certeza);
  void setSmallMazeMatPos(int xmazepos, int ymazepos, char val,double certeza);
  void setSmallMazeMatPosAfterProcess(int xmazepos, int ymazepos, char val,double certeza);
  
  double compareWithSmallMaze(char s[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF],
			      int  ssamples[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF],
			      int dx,int dy);
  void decreaseSmallMazeSamples();

  // variaveis

  WorldState * ws;
  
  char aux[LAB_X*2*MAZE_DEF][LAB_Y*2*MAZE_DEF];
  char aux2[LAB_X*2*MAZE_DEF][LAB_Y*2*MAZE_DEF];
  char maze[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  char mazeErodedAndDilated[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  int samples[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  
  char robotMatrix[MAZE_DEF][MAZE_DEF];
  
  // neste caso é restringido pela definição da matriz
  char sensor[360][MAZE_DEF+1][3*MAZE_DEF][3*MAZE_DEF];
  
  // neste caso é restringido pela definição do sensor
  
  matrix sensor_long[360][10];
  
  /* funções de probabilidade */
  double prob_acc[200];
  
  int MATminXpossible;
  int MATmaxXpossible;
  int MATminYpossible;
  int MATmaxYpossible; 

  double maxXpossible;
  double minXpossible;
  double maxYpossible;
  double minYpossible;
  
  bool havePerfect;

  double isGoodToGoWithMouseMatrix[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  int isKnownMatrix[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  int isCanGoWithMouseMatrix[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  
  // optimizações de aplicação de matriz
  int good_mouse_matrix_size;
  int good_mouse_matrix[(2*GOOD_MOUSE_MARGIN+1)*(2*GOOD_MOUSE_MARGIN+1)][3];
  int go_mouse_matrix_size;
  int go_mouse_matrix[(2*GO_MOUSE_MARGIN+1)*(2*GO_MOUSE_MARGIN+1)][2];
  int known_matrix_size;
  int known_matrix[(2*KNOWN_MARGIN+1)*(2*KNOWN_MARGIN+1)][2];
  int valid_go_matrix_size;
  int valid_go_matrix[(2*VALID_GO_MARGIN+1)*(2*VALID_GO_MARGIN+1)][2];

  /********************************************************/
  /* SMALL WORLD STATE vars                               */
  /********************************************************/
  
  char Smallaux[LAB_X*2*MAZE_DEF][LAB_Y*2*MAZE_DEF];
  char Smallaux2[LAB_X*2*MAZE_DEF][LAB_Y*2*MAZE_DEF];
  char Smallmaze[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  char SmallmazeErodedAndDilated[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  int  Smallsamples[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];

};

int good_mouse_matrix_cmp(const void *a, const void *b);

inline bool WSmaze::inBounds(double x,double y)
{
  return (!(x >= maxXpossible || x < minXpossible) &&
	  !(y >= maxYpossible || y < minYpossible));
}

inline bool WSmaze::inMatrixBounds(int x,int y)
{
  return (!(x < MATminXpossible || x >= MATmaxXpossible) &&
	  !(y < MATminYpossible || y >= MATmaxYpossible));
}

inline void WSmaze::setToBounds(Position &pos)
{
  
  if (pos.getX() > maxXpossible - 0.8)
    pos.setX(maxXpossible - 0.8);
  if (pos.getX() < minXpossible + 0.8)
    pos.setX(minXpossible + 0.8);
  if (pos.getY() > maxYpossible - 0.8)
    pos.setY(maxYpossible - 0.8);
  if (pos.getY() < minYpossible + 0.8)
    pos.setY(minYpossible + 0.8);
}


#endif
