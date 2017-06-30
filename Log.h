#ifndef _LOG_H_
#define _LOG_H_

class Log;

#include "Action.h"
#include "LibComm.h"
#include "MathTools.h"
#include "WorldState.h"
#include "defines.h"
#include "viewer/WSviewer.h"
#include <sys/time.h>
#include <time.h>

using namespace std;

class Log {

public:
    Log(WorldState* w, bool state);
    ~Log();
    void init(void);
    void update(void);
    void doneActionLog();
    void writeDebug(const char* s);

private:
    FILE* logfile;
    bool enabled;
    WorldState* ws;

    bool haveInitXY;
    double Xini;
    double Yini;
    double Xpos;
    double Ypos;
    double DirDeg;

    static const int debug_n_lines = 100;

    char debug_text[debug_n_lines][255];
    int debug_text_first_line;

    friend class WSV;
};

#endif
