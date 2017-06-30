#include "WSviewer.h"

void* gui(void* world)
{
    ws_and_con_and_log* w = (ws_and_con_and_log*)world;
    WSV* wsv = new WSV(w->ws, w->con, w->log);
    wsv->run();

    return NULL;
}

WSV::WSV(WorldState* world, Connection* c, Log* l)
{
    con = c;
    ws = world;
    log = l;
    sws = new ServerWS(c);
    hasMouseEvent = false;
    pthread_t tid;
    pthread_create(&tid, NULL, tServerWS, (void*)sws);
    pthread_detach(tid);
}

void WSV::run()
{

    //// SDL init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    g_screenSurface = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);

    if (g_screenSurface == NULL) {
        printf("Unable to init video: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetCaption("XIP - WorldState Viewer", NULL);
    // Areas Init
    updateAreasSize();
    red = SDL_MapRGB(g_screenSurface->format, 255, 0, 0);
    green = SDL_MapRGB(g_screenSurface->format, 0, 255, 0);
    blue = SDL_MapRGB(g_screenSurface->format, 0, 0, 255);
    yellow = SDL_MapRGB(g_screenSurface->format, 255, 255, 0);
    black = SDL_MapRGB(g_screenSurface->format, 0, 0, 0);
    white = SDL_MapRGB(g_screenSurface->format, 255, 255, 255);
    gray = SDL_MapRGB(g_screenSurface->format, 64, 64, 64);
    light_gray = SDL_MapRGB(g_screenSurface->format, 200, 200, 200);
    ini = 0;
    d_ini = 0.0;

    offset_x = offset_y = 0.0;
    scale = 1.0 / 1.03;

    // run
    while (1) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEMOTION:
                dealWithMotion(event.motion);
                break;

            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_UP:
                    offset_y += 1.0;
                    break;
                case SDLK_DOWN:
                    offset_y -= 1.0;
                    break;
                case SDLK_RIGHT:
                    offset_x -= 1.0;
                    break;
                case SDLK_LEFT:
                    offset_x += 1.0;
                    break;
                case SDLK_KP_MINUS:
                case SDLK_MINUS:
                    scale /= 1.2;
                    break;
                case SDLK_KP_PLUS:
                case SDLK_PLUS:
                    scale *= 1.2;
                    break;
                case SDLK_SPACE:
                    sws->toStart = true;
                    sws->toStop = false;
                    break;
                case SDLK_RETURN:
                    sws->toStart = true;
                    sws->toStop = true;
                    break;
                case SDLK_PAGEDOWN:
                    if (d_ini < log->debug_n_lines - 17 - 0.5)
                        d_ini++;
                    break;
                case SDLK_PAGEUP:
                    if (d_ini > 0.5)
                        d_ini--;
                    break;
                default:
                    break;
                }
                break;

            case SDL_VIDEORESIZE:
                g_screenSurface = SDL_SetVideoMode(event.resize.w, event.resize.h, 32,
                    SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
                updateAreasSize();
                break;
            default:
                break;
            }
        }

        drawMazeWindow();
        drawDebugBar();
        drawStateBar();

        SDL_Flip(g_screenSurface);
    }

    ////// shut down
    SDL_FreeSurface(g_screenSurface);
    SDL_Quit();
}

void WSV::drawStateBar()
{
    SDL_FillRect(g_screenSurface, &state, black);

    //caixa da info dos beacons
    boxRGBA(g_screenSurface, state.x + 10, state.y + state.w + 10, state.x + state.w - 10, state.y + state.h - 10,
        64, 64, 64, 255);
    rectangleRGBA(g_screenSurface, state.x + 10, state.y + state.w + 10, state.x + state.w - 10, state.y + state.h - 10,
        255, 0, 0, 255);

    double center_x = state.x + state.w / 2, center_y = state.y + state.w / 2, r = state.w / 8;

    filledCircleRGBA(g_screenSurface, (int)center_x, (int)center_y, (int)r,
        0, 0, 255, 255);

    //sensores
    int sensor_x, sensor_y;
    int text_x, text_y;
    char s[100];

    //center left
    sprintf(s, "%2.01f", ws->getSensors()->getCenterLeftDist());
    sensor_x = (int)(center_x + r * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERLEFTPOS)));
    sensor_y = (int)(center_y - r * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERLEFTPOS)));
    text_x = (int)(center_x + r * 2.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERLEFTPOS)));
    text_y = (int)(center_y - r * 2.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERLEFTPOS)));
    filledPieRGBA(g_screenSurface, sensor_x, sensor_y,
        (int)(ws->getSensors()->getCenterLeftDist() * r),
        (int)-(CENTERLEFTPOS + 30 + ws->getPosition()->getDirDeg()),
        (int)-(CENTERLEFTPOS - 30 + ws->getPosition()->getDirDeg()),
        255, 255, 0, 64);
    stringRGBA(g_screenSurface, text_x - 16, text_y - 4, s,
        200, 200, 200, 255);

    //center right
    sprintf(s, "%2.01f", ws->getSensors()->getCenterRightDist());
    sensor_x = (int)(center_x + r * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERRIGHTPOS)));
    sensor_y = (int)(center_y - r * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERRIGHTPOS)));
    text_x = (int)(center_x + r * 2.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERRIGHTPOS)));
    text_y = (int)(center_y - r * 2.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(CENTERRIGHTPOS)));
    filledPieRGBA(g_screenSurface, sensor_x, sensor_y,
        (int)(ws->getSensors()->getCenterRightDist() * r),
        (int)-(CENTERRIGHTPOS + 30 + ws->getPosition()->getDirDeg()),
        (int)-(CENTERRIGHTPOS - 30 + ws->getPosition()->getDirDeg()),
        255, 255, 0, 64);
    stringRGBA(g_screenSurface, text_x - 16, text_y - 4, s,
        200, 200, 200, 255);

    //left
    sprintf(s, "%2.01f", ws->getSensors()->getLeftDist());
    sensor_x = (int)(center_x + r * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(LEFTPOS)));
    sensor_y = (int)(center_y - r * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(LEFTPOS)));
    text_x = (int)(center_x + r * 2.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(LEFTPOS)));
    text_y = (int)(center_y - r * 2.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(LEFTPOS)));
    filledPieRGBA(g_screenSurface, sensor_x, sensor_y,
        (int)(ws->getSensors()->getLeftDist() * r),
        (int)-(LEFTPOS + 30 + ws->getPosition()->getDirDeg()),
        (int)-(LEFTPOS - 30 + ws->getPosition()->getDirDeg()),
        255, 255, 0, 64);
    stringRGBA(g_screenSurface, text_x - 16, text_y - 4, s,
        200, 200, 200, 255);

    //right
    sprintf(s, "%2.01f", ws->getSensors()->getRightDist());
    sensor_x = (int)(center_x + r * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(RIGHTPOS)));
    sensor_y = (int)(center_y - r * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(RIGHTPOS)));
    text_x = (int)(center_x + r * 2.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(RIGHTPOS)));
    text_y = (int)(center_y - r * 2.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(RIGHTPOS)));
    filledPieRGBA(g_screenSurface, sensor_x, sensor_y,
        (int)(ws->getSensors()->getRightDist() * r),
        (int)-(RIGHTPOS + 30 + ws->getPosition()->getDirDeg()),
        (int)-(RIGHTPOS - 30 + ws->getPosition()->getDirDeg()),
        255, 255, 0, 64);
    stringRGBA(g_screenSurface, text_x - 16, text_y - 4, s,
        200, 200, 200, 255);
}

void WSV::drawMazeWindow()
{
    // Actualizar escala dos pixeis
    true_lab_w = ws->getPosition()->getMaxXpossible() - ws->getPosition()->getMinXpossible();
    true_lab_h = ws->getPosition()->getMaxYpossible() - ws->getPosition()->getMinYpossible();

    if (maze.w / true_lab_w > maze.h / true_lab_h) {
        lab_h = (int)(maze.h * scale);
        lab_w = (int)((true_lab_w * maze.h / true_lab_h) * scale);
        margin_y = (int)((maze.h - lab_h) / 2 + offset_y * lab_h / true_lab_h);
        margin_x = (int)((maze.w - lab_w) / 2 + offset_x * lab_w / true_lab_w);
        np = (int)(lab_h / (true_lab_h * MAZE_DEF));
    } else {
        lab_w = (int)(maze.w * scale);
        lab_h = (int)((true_lab_h * maze.w / true_lab_w) * scale);
        margin_x = (int)((maze.w - lab_w) / 2 + offset_x * lab_w / true_lab_w);
        margin_y = (int)((maze.h - lab_h) / 2 + offset_y * lab_h / true_lab_h);
        np = (int)(lab_w / (true_lab_w * MAZE_DEF));
    }

    SDL_FillRect(g_screenSurface, &maze, black);
    drawMaze();
    drawBeacons();
    //drawAstar();
    drawMazeMouse();
    drawPath();
}

void WSV::drawAstar()
{
    bool val;
    double j = 0, i = 0, inc = 1.0 / MAZE_DEF;

    for (j = ws->getPosition()->getMinYpossible() + 0.5; j < ws->getPosition()->getMaxYpossible() - 0.5; j += inc)
        for (i = ws->getPosition()->getMinXpossible() + 0.5; i < ws->getPosition()->getMaxXpossible() - 0.5; i += inc) {
            if (!ws)
                continue;
            val = (ws->isAstarPosExp(i, j));

            if (val) {
                filledCircleRGBA(g_screenSurface, xToScreenPos(i), yToScreenPos(j),
                    np, 255, 185, 15, 32);
            }
        }
}

void WSV::drawPath()
{
    Position ant, cur;
    if (ws->getPlan() == NULL)
        return;

    // a* path
    if (ws->getPlan()->routeToDestination.size()) {
        ant = ws->getPlan()->routeToDestination[0].getPosition();
        for (unsigned i = 1; ws->getPlan() != NULL && i < ws->getPlan()->routeToDestination.size() && i < 1000; i++) {
            cur = ws->getPlan()->routeToDestination[i].getPosition();
            lineRGBA(g_screenSurface, xToScreenPos(ant.getX()), yToScreenPos(ant.getY()),
                xToScreenPos(cur.getX()), yToScreenPos(cur.getY()), 0, 150, 0, 255);
            ant = cur;
        }
    }

    //optimized path
    if (ws->getPlan()->optimizedRouteToDestination.size()) {
        ant = ws->getPlan()->optimizedRouteToDestination[0].getPosition();
        for (unsigned i = 1; ws->getPlan() != NULL && i < ws->getPlan()->optimizedRouteToDestination.size() && i < 1000; i++) {
            cur = ws->getPlan()->optimizedRouteToDestination[i].getPosition();
            lineRGBA(g_screenSurface, xToScreenPos(ant.getX()), yToScreenPos(ant.getY()),
                xToScreenPos(cur.getX()), yToScreenPos(cur.getY()), 255, 140, 0, 255);
            ant = cur;
        }
    }

    if (ws->getPlan() != NULL && ws->getPlan()->optimizedRouteToDestination.size()) {
        ant = ws->getPlan()->optimizedRouteToDestination[0].getPosition();
        for (unsigned i = 1; ws->getPlan() != NULL && i < ws->getPlan()->optimizedRouteToDestination.size() && i < 1000; i++) {
            cur = ws->getPlan()->optimizedRouteToDestination[i].getPosition();
            filledCircleRGBA(g_screenSurface, xToScreenPos(cur.getX()), yToScreenPos(cur.getY()),
                2 * np, 255, 140, 0, 255);
            ant = cur;
        }
    }

    /*if (hasMouseEvent) {

	//BEST PATH until point
	double percent_pos = ((double)ini/(log->debug_n_lines-17));
	filledCircleRGBA(g_screenSurface,debug.x+debug.w-10 ,(int)(debug.y+16+(debug.h-32)*percent_pos), 4, 255,255,0,255);

	if (!ws->getMaze()->inBounds(xToMazePos(mouseEvent.x),yToMazePos(mouseEvent.y))) return;
	Path * node = ws->getPlan()->getPath(DiscretePosition(Position(xToMazePos(mouseEvent.x),yToMazePos(mouseEvent.y))));
	if (node==NULL || !node->isExpansioned()) return;
	Position cur,ant = node->getPosition().getPosition();
	while(node!=NULL && node->isExpansioned())
	{
	cur = Position(node->getPosition().getPosition());
	lineRGBA(g_screenSurface, xToScreenPos(cur.getX()), yToScreenPos(cur.getY()),
	xToScreenPos(ant.getX()),yToScreenPos(ant.getY()), 255, 0,0,255);
	ant=cur;
	node = node->getBefore();
	}

	//hasMouseEvent=false;
	}*/
}

void WSV::drawDebugBar()
{
    SDL_FillRect(g_screenSurface, &debug, black);

    //caixa da info dos beacons
    boxRGBA(g_screenSurface, debug.x + 10, debug.y + 10, debug.x + debug.w - 10, debug.y + debug.h - 10,
        64, 64, 64, 255);
    rectangleRGBA(g_screenSurface, debug.x + 10, debug.y + 10, debug.x + debug.w - 10, debug.y + debug.h - 10,
        255, 0, 0, 255);

    // Preencher buffer de texto do debug
    ini = (int)round(d_ini);
    int chars_per_line = (debug.w - 16 * 2) / 8; // largura do char + espaço entre chars = 9
    if (chars_per_line < 2)
        chars_per_line = 2;
    int log_line = ini;

    for (int i = 0; i < 17;) {
        int line2_print = ((log->debug_text_first_line) - (log_line++) + (log->debug_n_lines)) % (log->debug_n_lines);
        int line_len = strlen(log->debug_text[line2_print]);
        int n_wrote = 0;
        debug_buf[i][0] = '\0';
        if (line_len == 0) {
            i++;
            continue;
        }
        while (n_wrote < line_len) {
            strncpy(debug_buf[i], &(log->debug_text[line2_print][n_wrote]), min(line_len - n_wrote, chars_per_line));
            debug_buf[i++][min(line_len - n_wrote, chars_per_line)] = '\0';
            n_wrote += chars_per_line;
        }
    }

    for (int i = 0; i < 17; i++) {
        stringRGBA(g_screenSurface, debug.x + 16, debug.y + 16 + 10 * i,
            debug_buf[i],
            200, 200, 200, 255);
    }

    // SCROLL
    double percent_pos = ((double)ini / (log->debug_n_lines - 17));
    filledCircleRGBA(g_screenSurface, debug.x + debug.w - 10, (int)(debug.y + 16 + (debug.h - 32) * percent_pos), 4, 255, 255, 0, 255);
}

void WSV::updateAreasSize()
{
    // state bar
    state.x = g_screenSurface->w - state_width;
    state.y = 0;
    state.w = state_width;
    state.h = g_screenSurface->h;

    // debug Bar
    debug.x = 0;
    debug.y = g_screenSurface->h - debug_height;
    debug.w = g_screenSurface->w - state_width;
    debug.h = debug_height;

    // maze window
    maze.x = 0;
    maze.y = 0;
    maze.w = g_screenSurface->w - state_width;
    maze.h = g_screenSurface->h - debug_height;
}

void WSV::drawMaze()
{
    SDL_Rect maze;

    Uint32 color = white;

    int val;
    double j = 0, i = 0, inc = 1.0 / MAZE_DEF;
    for (j = ws->getPosition()->getMinYpossible(); j <= ws->getPosition()->getMaxYpossible(); j += inc)
        for (i = ws->getPosition()->getMinXpossible(); i <= ws->getPosition()->getMaxXpossible(); i += inc) {
            val = (ws->getMaze()->getMazePos(i, j));
            if (val < 0)
                color = gray;
            else if (val < 30 && val >= 0)
                color = light_gray;
            else if (val > 80)
                color = red;
            else
                color = yellow;

            maze.x = xToScreenPos(i) - np / 2;
            maze.y = yToScreenPos(j) + np / 2;
            maze.w = np + 1;
            maze.h = np + 1;

            SDL_FillRect(g_screenSurface, &maze, color);
        }

    maze.w = xToScreenPos(i);
    maze.h = yToScreenPos(j - inc);
    maze.x = xToScreenPos(ws->getPosition()->getMinXpossible()) - np / 2;
    maze.y = yToScreenPos(ws->getPosition()->getMinYpossible() - inc) + np / 2;

    rectangleRGBA(g_screenSurface, maze.x, maze.y, maze.w, maze.h, 255, 0, 0, 255);
}

void WSV::drawBeaconMat(int beacon_i)
{
    SDL_Rect maze;

    SDL_Surface* g_beaconSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, g_screenSurface->w, g_screenSurface->h, 32,
        0, 0, 0, 128);
    SDL_FillRect(g_beaconSurface, 0, SDL_MapRGBA(g_beaconSurface->format, 0, 0, 0, 0));

    Uint32 color = white;
    int val;
    double j = 0, i = 0, inc = 1.0 / MAZE_DEF;
    for (j = ws->getPosition()->getMinYpossible(); j <= ws->getPosition()->getMaxYpossible(); j += inc)
        for (i = ws->getPosition()->getMinXpossible(); i <= ws->getPosition()->getMaxXpossible(); i += inc) {
            maze.x = xToScreenPos(i) - np / 2;
            maze.y = yToScreenPos(j) + np / 2;
            maze.w = np + 1;
            maze.h = np + 1;

            val = (ws->getBeacon(beacon_i)->getVal(i, j));
            if (val <= 0)
                continue;
            if (val > 100)
                val = 100;
            val = val * 255 / 100;
            color = SDL_MapRGB(g_beaconSurface->format, 255 - val, 255, 255 - val);

            SDL_FillRect(g_beaconSurface, &maze, color);
        }
    SDL_SetColorKey(g_beaconSurface, SDL_SRCCOLORKEY, SDL_MapRGB(g_beaconSurface->format, 0, 0, 0));
    SDL_SetAlpha(g_beaconSurface, SDL_SRCALPHA, 128);
    SDL_BlitSurface(g_beaconSurface, 0, g_screenSurface, 0);
    SDL_FreeSurface(g_beaconSurface);
}
int WSV::xToScreenPos(double x)
{
    x -= (ws->getPosition()->getMinXpossible());
    x = lab_w * x / true_lab_w;
    return (int)(x + margin_x);
}

int WSV::yToScreenPos(double y)
{
    y -= (ws->getPosition()->getMinYpossible());
    y = -lab_h * y / true_lab_h + lab_h;
    return (int)(y + margin_y);
}

double WSV::xToMazePos(int px)
{
    double x;
    px = px - margin_x;
    x = ((double)px * true_lab_w) / lab_w;
    return x + (ws->getPosition()->getMinXpossible());
}

double WSV::yToMazePos(int py)
{
    double y;
    py = py - margin_y;
    y = -((py - lab_h) * true_lab_h) / lab_h;
    return y + (ws->getPosition()->getMinYpossible());
}

void WSV::drawMazeMouse()
{
    if (ws->getSensors()->inCollision() || ws->getSensors()->wasInCollision())
        filledCircleRGBA(g_screenSurface, xToScreenPos(ws->getPosition()->getX()), yToScreenPos(ws->getPosition()->getY()), (int)(lab_h / (true_lab_h * 2)), 255, 0, 0, 255);
    else
        filledCircleRGBA(g_screenSurface, xToScreenPos(ws->getPosition()->getX()), yToScreenPos(ws->getPosition()->getY()), (int)(lab_h / (true_lab_h * 2)), 0, 0, 255, 255);
}

void WSV::drawBeacons()
{
    for (int i = 0; i < ws->getParameters()->getNBeacons(); i++) {
        if (ws->getBeacon(i)->getProb()) {
            drawBeacon(255, 255, 0, ws->getBeacon(i)->getProb() * 32, ws->getBeacon(i)->getPosition());
            drawBeaconMat(i);
        }
    }
}

void WSV::drawBeacon(int r, int g, int b, int a, Position pos)
{
    filledCircleRGBA(g_screenSurface, xToScreenPos(pos.getX()), yToScreenPos(pos.getY()), (int)(lab_h * 2.0 / (true_lab_h * 2)), 0, 128, 0, a);
    filledCircleRGBA(g_screenSurface, xToScreenPos(pos.getX()), yToScreenPos(pos.getY()), (int)(lab_h * 1.5 / (true_lab_h * 2)), r, g, b, 128);
}

void WSV::dealWithMotion(SDL_MouseMotionEvent event)
{

    double percent_pos = ((double)ini / (log->debug_n_lines - 17));
    int xpos = (debug.x + debug.w - 10) - event.x;
    int ypos = (int)(debug.y + 16 + (debug.h - 32) * percent_pos) - event.y;
    int in_range = (xpos * xpos + ypos * ypos) < 50 || abs(ypos) < 20;

    if (in_range && event.state & SDL_BUTTON(1) == SDL_PRESSED) {
        double inc = (((double)event.yrel * (log->debug_n_lines - 17)) / (debug.h - 32));
        if ((d_ini + inc) >= 0 && (d_ini + inc < log->debug_n_lines - 17)) {
            d_ini += inc;
            ini = (int)round(d_ini);
        }
    }

    mouseEvent = event;
    hasMouseEvent = true;
}
