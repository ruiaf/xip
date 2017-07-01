#include "ArrowTool.h"

ArrowTool::ArrowTool(MazeCanvas * mc)
: MazeTool(ARROW_TOOL,mc)
{
	selected_object = NULL;
	last_x = last_y = 0;
}

ArrowTool::~ArrowTool()
{
	if(selected_object!= NULL)
	{
		delete selected_object;
		selected_object = NULL;
	}
}

void ArrowTool::paint(wxDC * dc,  int offset_x, int offset_y)
{
	if (selected_object != NULL)
		selected_object->paint(dc,offset_x, offset_y);
}

void ArrowTool::mouseLeftButtonDown(wxMouseEvent & event)
{
	if (selected_object != NULL)
		resetTool();

	selected_object = maze_canvas->getSelectedObject(event.m_x, event.m_y);


	if (selected_object != NULL)
	{
		selected_object->setSelected(true);
		last_x = event.m_x;
		last_y = event.m_y;
		maze_canvas->Refresh();
	}
}

void ArrowTool::mouseLeftButtonUp(wxMouseEvent & event)
{
	/*if (selected_object != NULL)
	{
		selected_object->setSelected(false);
		maze_canvas->addNewObject(selected_object);
		selected_object = NULL;
		maze_canvas->Refresh();
		last_x = last_y = 0;
	}*/	
}

void ArrowTool::mouseMoved(wxMouseEvent & event)
{
	if ( event.LeftIsDown() && selected_object != NULL)
	{
		selected_object->translateBy(event.m_x-last_x, event.m_y-last_y);
		/*if(!selected_object->mazeObjectInsideRectangle(0,0,maze_canvas->getWindowCurrentWidth(), maze_canvas->getWindowCurrentHeight()))
			selected_object->translateBy(-event.m_x+last_x, -event.m_y+last_y);*/
		last_x = event.m_x;
		last_y = event.m_y;
		maze_canvas->Refresh();
	}
}

void ArrowTool::resetTool(void)
{
	if (selected_object != NULL)
	{
		selected_object->setSelected(false);
		maze_canvas->addNewObject(selected_object);
		selected_object = NULL;
		last_x = last_y = 0;
		maze_canvas->Refresh();
	}
}

void ArrowTool::mouseRightButtonDown(wxMouseEvent & event)
{
	resetTool();
}

void ArrowTool::keyDown(wxKeyEvent & event)
{
	if (selected_object!=NULL && 
		(event.GetKeyCode() == WXK_DELETE || WXK_BACK  == event.GetKeyCode()))
	{
		MazeObject * temp = selected_object;
		selected_object = NULL;
		delete temp;
		maze_canvas->verifyConstraints();
		maze_canvas->Refresh();
	}
}

	
void ArrowTool::mouseLeftDoubleClick(wxMouseEvent & event)
{
	if (selected_object != NULL)
		resetTool();

	selected_object = maze_canvas->getSelectedObject(event.m_x, event.m_y);

	if (selected_object != NULL)
	{
		selected_object->setSelected(true);
		maze_canvas->Refresh();
		selected_object->showPropertiesBox();
		last_x = event.m_x;
		last_y = event.m_y;
	}
}


