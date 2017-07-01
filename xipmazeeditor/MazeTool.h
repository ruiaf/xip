#ifndef _MAZETOOL_H_
#define _MAZETOOL_H_

#include "wx/dc.h"

#include "MazeCanvas.h"

class MazeTool
{
public:
	
	MazeTool(int type, MazeCanvas * maze_canvas) : tool_type(type), maze_canvas(maze_canvas){}

	virtual void paint(wxDC * dc,  int offset_x, int offset_y)=0;

	virtual void mouseLeftButtonDown(wxMouseEvent & event)=0;	
	virtual void mouseLeftButtonUp(wxMouseEvent & event)=0;	
	virtual void mouseLeftDoubleClick(wxMouseEvent & event)=0;
	virtual void mouseRightButtonDown(wxMouseEvent & event)=0;
	virtual void mouseMoved(wxMouseEvent & event)=0;	
	virtual void keyDown(wxKeyEvent & event)=0;

	virtual void resetTool(void)=0;

	int getToolType(void){return tool_type;}
	void setToolType(int new_tool_type){tool_type = new_tool_type;}


protected:
	int tool_type;
	MazeCanvas * maze_canvas;

private:
	MazeTool(){}
};

#endif
