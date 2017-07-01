#ifndef _WALLTOOL_H_
#define _WALLTOOL_H_


#include "Defines.h"
#include "DefaultConfs.h"
#include "MazeCanvas.h"
#include "MazeTool.h"
#include "Wall.h"

class MazeCanvas;

class WallTool : public MazeTool
{

public:	
	WallTool(MazeCanvas * mc, float wall_height);
	~WallTool();

	void paint(wxDC * dc, int offset_x, int offset_y);

	void mouseLeftButtonDown(wxMouseEvent & event);	
	void mouseLeftButtonUp(wxMouseEvent & event);	
	void mouseLeftDoubleClick(wxMouseEvent & event);
	void mouseRightButtonDown(wxMouseEvent & event);
	void mouseMoved(wxMouseEvent & event);
	void keyDown(wxKeyEvent & event);

	void resetTool(void);

protected:

	Wall * temp_wall;
	float wall_height;
};


#endif

