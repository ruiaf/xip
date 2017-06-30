#include "Log.h"

Log::Log(WorldState *w, bool state)
{
	struct timeval t;
	struct timezone tz;
	char name[256],nname[256];

	enabled=state;
	ws = w;
	
	if (enabled)
	{
		gettimeofday(&t,&tz);
		strftime(name,256,"%Y_%m_%d_%H_%M",localtime(&t.tv_sec));
		sprintf(nname,"logfiles/xip_%s.log",name);

		puts(nname);
		logfile = fopen(nname,"w");
		if (logfile < 0)
			printf("could not create log: %s\n",name);
		haveInitXY=false;
	}
	// debug init
  
	debug_text_first_line = 0;
	memset(debug_text,0,sizeof(debug_text));
}

Log::~Log()
{
	if (enabled)
		fclose(logfile);
}


void Log::init(void)
{
	if (enabled)
	{
		Xini = GetX();
		Yini = GetY();
		haveInitXY=true;
	}
}

void Log::update(void)
{
	if (enabled)
	{
		Xpos = GetX();
		Ypos = GetY();
		DirDeg = GetDir();
	}
}

void Log::doneActionLog(void)
{
}

void Log::writeDebug(const char *s)
{
  debug_text_first_line = (debug_text_first_line+1) % debug_n_lines;	
  sprintf(debug_text[debug_text_first_line],"%d: %s",ws->getTime(),s);
}


