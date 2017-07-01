#include "StartingGridTool.h"

StartingGridTool::StartingGridTool(MazeCanvas * mc)
: MazeTool(STARTING_GRID_TOOL,mc)
{
	temp_starting_grid = NULL;
	setingDirection = false;
}

StartingGridTool::~StartingGridTool()
{
	resetTool();
}

void StartingGridTool::paint(wxDC * dc, int offset_x, int offset_y)
{
	if (temp_starting_grid != NULL)
		temp_starting_grid->paint(dc,offset_x,offset_y);
}

void StartingGridTool::mouseLeftButtonDown(wxMouseEvent & event)
{
	if (temp_starting_grid != NULL)
	{
		if (setingDirection)
		{
			MazeObject * temp = temp_starting_grid;
			temp_starting_grid = NULL;
			maze_canvas->addNewObject(temp);
			resetTool();
			maze_canvas->Refresh();
		}
		else
		{
			setingDirection = true;
		}
	}
	else
	{
		temp_starting_grid = new StartingGrid(maze_canvas->getFirstStartingGridUnusedNumber(), event.m_x, event.m_y);
	}
}

void StartingGridTool::mouseLeftButtonUp(wxMouseEvent & event)
{	
}

void StartingGridTool::mouseMoved(wxMouseEvent & event)
{
	if (temp_starting_grid != NULL)
	{
		if (setingDirection)
		{
			temp_starting_grid->setDirection((double)event.m_x, (double)event.m_y, 0, 0);
			maze_canvas->Refresh();
		}
		else
		{
			temp_starting_grid->setPosition(event.m_x, event.m_y);
			maze_canvas->Refresh();
		}
	}
	else
	{
		temp_starting_grid = new StartingGrid(maze_canvas->getFirstStartingGridUnusedNumber(), event.m_x, event.m_y);
	}
}

void StartingGridTool::resetTool(void)
{
	if (temp_starting_grid != NULL)
	{
		MazeObject * temp = temp_starting_grid;
		temp_starting_grid = NULL;
		delete temp;
	}
	setingDirection = false;
		
}

void StartingGridTool::mouseRightButtonDown(wxMouseEvent & event)
{
	maze_canvas->setActiveTool(ARROW_TOOL);
}


void StartingGridTool::keyDown(wxKeyEvent & event)
{
}

	
void StartingGridTool::mouseLeftDoubleClick(wxMouseEvent & event)
{
}
