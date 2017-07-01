#ifndef _ARROWTOOL_H_
#define _ARROWTOOL_H_


#include "Defines.h"
#include "MazeCanvas.h"
#include "MazeObject.h"
#include "MazeTool.h"


class ArrowTool : public MazeTool
{

public:	
	ArrowTool(MazeCanvas * mc);
	~ArrowTool();

	void paint(wxDC * dc,  int offset_x, int offset_y);

	void mouseLeftButtonDown(wxMouseEvent & event);	
	void mouseLeftButtonUp(wxMouseEvent & event);	
	void mouseLeftDoubleClick(wxMouseEvent & event);
	void mouseRightButtonDown(wxMouseEvent & event);
	void mouseMoved(wxMouseEvent & event);
	void keyDown(wxKeyEvent & event);

	void resetTool(void);

protected:

	MazeObject * selected_object;
	int last_x, last_y;
};


#endif
