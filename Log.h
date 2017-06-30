#ifndef _LOG_H_
#define _LOG_H_

class Log;

#include "MathTools.h"
#include "LibComm.h"
#include "Action.h"
#include "WorldState.h"
#include "defines.h"
#include <sys/time.h>
#include <time.h>
#include "viewer/WSviewer.h"

using namespace std;

class Log
{

	public:
		Log(WorldState *w,bool state);
		~Log();
		void init(void);
		void update(void);
		void doneActionLog();
		void writeDebug(const char *s);


	private:
		FILE * logfile;
		bool enabled;
		WorldState *ws;

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
