#ifndef _DEFAULTCONFS_H_
#define _DEFAULTCONFS_H_

#include "wx/file.h"
#include "wx/string.h"


#include "Defines.h"


class DefaultConfs
{
public:
	static void loadDefaultConfsFromFile();
	static double getDEFAULT_CANVAS_WIDTH();
	static double getDEFAULT_CANVAS_HEIGHT();
	static double getMAXIMUM_LOW_WALL_HEIGHT();
	static double getDEFAULT_LOW_WALL_HEIGHT(); 
	static double getDEFAULT_HIGH_WALL_HEIGHT(); 
	static double getDEFAULT_BEACON_RADIUS();
	static double getDEFAULT_BEACON_HEIGHT(); 
	static double getSTARTING_GRID_PAINTED_SQUARE_WIDTH();
	static double getSTARTING_GRID_PAINTED_SQUARE_HEIGHT();
	static double getDEFAULT_ZOOM_FACTOR();
	static double getEXPORT_RESOLUTION();
	
private:

	static void interpretLine(wxString & line);

	static double DEFAULT_CANVAS_WIDTH; 
	static double DEFAULT_CANVAS_HEIGHT; 
	static double MAXIMUM_LOW_WALL_HEIGHT;
	static double DEFAULT_LOW_WALL_HEIGHT; 
	static double DEFAULT_HIGH_WALL_HEIGHT; 
	static double DEFAULT_BEACON_RADIUS;
	static double DEFAULT_BEACON_HEIGHT; 
	static double STARTING_GRID_PAINTED_SQUARE_WIDTH;
	static double STARTING_GRID_PAINTED_SQUARE_HEIGHT;
	static double DEFAULT_ZOOM_FACTOR;
	static double EXPORT_RESOLUTION;
};

#endif

