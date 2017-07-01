#ifndef _STARTINGGRIDTOOL_H_
#define _STARTINGGRIDTOOL_H_


#include "Defines.h"
#include "MazeCanvas.h"
#include "MazeTool.h"
#include "StartingGrid.h"


class StartingGridTool : public MazeTool
{

public:	
	StartingGridTool(MazeCanvas * mc);
	~StartingGridTool();

	void paint(wxDC * dc, int offset_x, int offset_y);

	void mouseLeftButtonDown(wxMouseEvent & event);	
	void mouseLeftButtonUp(wxMouseEvent & event);	
	void mouseLeftDoubleClick(wxMouseEvent & event);	
	void mouseRightButtonDown(wxMouseEvent & event);
	void mouseMoved(wxMouseEvent & event);
	void keyDown(wxKeyEvent & event);

	void resetTool(void);

protected:

	StartingGrid * temp_starting_grid;
	
	bool setingDirection;
};


#endif
