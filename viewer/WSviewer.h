#ifndef _WSVIEW_H_
#define _WSVIEW_H_

#include "../Connection.h"
#include "../Position.h"
#include "../WorldState.h"
#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"
#include "ServerWS.h"

typedef struct
{
    WorldState* ws;
    Connection* con;
    Log* log;
} ws_and_con_and_log;

void* gui(void* w);

class WSV {

public:
    WSV(WorldState* ws, Connection* c, Log* log);
    void run();

private:
    void drawStateBar();
    void drawMazeWindow();
    void drawDebugBar();
    void updateAreasSize();
    void drawMaze();
    void drawMazeMouse();
    void drawBeacons();
    void drawBeaconMat(int beacon_i);
    void drawBeacon(int r, int g, int b, int a, Position pos);

    void drawPath();
    void drawAstar();

    int xToScreenPos(double x);
    int yToScreenPos(double y);
    double xToMazePos(int px);
    double yToMazePos(int py);

    void dealWithMotion(SDL_MouseMotionEvent event);
    SDL_Surface* g_screenSurface;
    WorldState* ws;
    Connection* con;
    Log* log;

    static const int state_width = 200;
    static const int debug_height = 200;

    SDL_Rect state;
    SDL_Rect debug;
    SDL_Rect maze;

    Uint32 red;
    Uint32 green;
    Uint32 blue;
    Uint32 white;
    Uint32 gray;
    Uint32 light_gray;
    Uint32 black;
    Uint32 yellow;

    int margin_x;
    int margin_y;
    int lab_w;
    int lab_h;
    int np;

    double offset_x;
    double offset_y;
    double scale;

    double true_lab_w;
    double true_lab_h;

    ServerWS* sws;

    bool hasMouseEvent;
    SDL_MouseMotionEvent mouseEvent;

    char debug_buf[20][255];
    int ini;
    double d_ini;
};

#endif
