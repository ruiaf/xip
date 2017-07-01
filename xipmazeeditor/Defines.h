#ifndef _DEFINES_H_
#define _DEFINES_H_

/***** Classes ********************************************/

class Beacon;
class BeaconPropertiesBox;
class BeaconTool;
class DefaultConfs;
class ExportDialog;
class MazeCanvas;
class MazeCanvasPropertiesBox;
class MazeFileDialog;
class MazeFileHandler;
class MazeXMLTag;
class MazeXMLTagAttribute;
class MyApp;
class MyFrame;
class Polygon;
class StartingGrid;
class StartingGridPropertiesBox;
class StartingGridTool;
class Wall;
class WallPropertiesBox;
class WallTool;

#include "wx/event.h"

#include "wx/dialog.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#define debug4d(a,b,c,d) { wxString blablabla; blablabla.Printf("%d %d %d %d",a,b,c,d); wxDialog(NULL, -1, blablabla, wxPoint(0,0), wxSize(400,100)).ShowModal();}
#define debug1s(a) { wxString blablabla(a); wxDialog(NULL, -1, blablabla, wxPoint(0,0), wxSize(400,100)).ShowModal();}


/***** Global Definitions *********************************/

#define CONF_FILE_PATH "conf/maze_editor.conf"

#define MAX_FILE_PATH_SIZE 2048

#define READ_BUFFER_SIZE 1024
#define WRITE_BUFFER_SIZE 1024

#define DEFAULT_GRID_PIXEL_INCREMENT (DefaultConfs::getDEFAULT_ZOOM_FACTOR()*0.5)

#define SCROLL_INCREMENT 1

#define PI acos(-1)

/***** Images *********************************************/

#define IMAGE_WINDOW_ICON "images/icon.bmp"
#define IMAGE_TOOLBAR_BEACON "images/toolbar_beacon.bmp"
#define IMAGE_TOOLBAR_LOW_WALL "images/toolbar_low_wall.bmp"
#define IMAGE_TOOLBAR_HIGH_WALL "images/toolbar_high_wall.bmp"
#define IMAGE_TOOLBAR_STARTING_GRID "images/toolbar_starting_grid.bmp"
#define IMAGE_TOOLBAR_ARROW "images/toolbar_arrow.bmp"



enum
{
	TYPE_BEACON,
	TYPE_HIGH_WALL,
	TYPE_LOW_WALL,
	TYPE_STARTING_GRID
};


/****** Tool Definitions *********************************/
#define TOOLBARTOOL_ID_OFFSET 20
#define TOOL_QUANTITY 5


/****** Tool types ***************************************/

enum mazetools
{
	NO_TOOL=-1,
	BEACON_TOOL=0 ,
	STARTING_GRID_TOOL,
	HIGH_WALL_TOOL,
	LOW_WALL_TOOL,
	ARROW_TOOL
};

/****** Get Tool Id's for the onclick events *************/

#define TOOLBARTOOL_ID(a) (a+TOOLBARTOOL_ID_OFFSET)

/***** Macros ********************************************/

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

/***** MazeFileDialog ************************************/

enum{
	ok_button_id=1,
	cancel_button_id,
	maze_file_path_button_id,
	starting_grid_file_path_button_id,
	scroll_window_id
};


#endif

