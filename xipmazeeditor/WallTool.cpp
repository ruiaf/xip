#include "WallTool.h"

WallTool::WallTool(MazeCanvas * mc, float wall_height)
:MazeTool(TOOLBARTOOL_ID(NO_TOOL), mc), wall_height(wall_height)
{
	temp_wall=NULL;
	if (wall_height > DefaultConfs::getMAXIMUM_LOW_WALL_HEIGHT())
		setToolType(HIGH_WALL_TOOL);
	else
		setToolType(LOW_WALL_TOOL);
}

WallTool::~WallTool()
{
	if (temp_wall != NULL)
	{
		delete temp_wall;
		temp_wall=NULL;
	}
}

void WallTool::paint(wxDC * dc, int offset_x, int offset_y)
{
	if (temp_wall != NULL)
		temp_wall->paint(dc,offset_x,offset_y);
}

void WallTool::mouseLeftButtonDown(wxMouseEvent & event)
{
	if (temp_wall != NULL)
	{		
		if (temp_wall->inFirstVertexArea(event.m_x, event.m_y))
		{	
			if (!temp_wall->valid())
				return;
			temp_wall->close();
			maze_canvas->addNewObject(temp_wall);
			temp_wall=NULL;
			resetTool();
			maze_canvas->Refresh();
		}
		else
		{
			temp_wall->addPoint(event.m_x, event.m_y);
		}
	}
}

void WallTool::mouseLeftButtonUp(wxMouseEvent & event)
{
}

void WallTool::mouseMoved(wxMouseEvent & event)
{
	if (temp_wall != NULL)
	{		
		temp_wall->inFirstVertexArea(event.m_x, event.m_y);
		temp_wall->setNextVertexPosition(event.m_x, event.m_y);
		maze_canvas->Refresh();
	}
}


void WallTool::resetTool(void)
{
	if (temp_wall != NULL)
	{
		delete temp_wall;
		temp_wall = NULL;
	}
	temp_wall = new Wall(wall_height);
}

void WallTool::mouseRightButtonDown(wxMouseEvent & event)
{
	if (temp_wall != NULL)
	{
		if (temp_wall->getNumberOfVertexes() > 0)
			resetTool();
		else
			maze_canvas->setActiveTool(ARROW_TOOL);
		maze_canvas->Refresh();
	}	
}



void WallTool::keyDown(wxKeyEvent & event)
{
}


void WallTool::mouseLeftDoubleClick(wxMouseEvent & event)
{
}

