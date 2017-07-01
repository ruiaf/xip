#include "BeaconTool.h"

BeaconTool::BeaconTool(MazeCanvas * mc) : MazeTool(BEACON_TOOL, mc)
{
	temp_beacon = NULL;
}

BeaconTool::~BeaconTool()
{
	resetTool(); 
}

void BeaconTool::paint(wxDC * dc, int offset_x, int offset_y)
{
	if (temp_beacon != NULL)
		temp_beacon->paint(dc,offset_x,offset_y);
}

void BeaconTool::mouseLeftButtonDown(wxMouseEvent & event)
{
	if (temp_beacon!=NULL)
	{
		MazeObject * temp = temp_beacon;
		temp_beacon=NULL;
		maze_canvas->addNewObject(temp);		
		resetTool();
		maze_canvas->Refresh();
	}
	else
	{
		temp_beacon = new Beacon(event.m_x, event.m_y, (float)(DefaultConfs::getDEFAULT_BEACON_RADIUS() * maze_canvas->getZoomFactor()));
	}
}

void BeaconTool::mouseLeftButtonUp(wxMouseEvent & event)
{
}

void BeaconTool::mouseMoved(wxMouseEvent & event)
{
	if (temp_beacon!=NULL)
	{	
		temp_beacon->setPosition(event.m_x, event.m_y);
		maze_canvas->Refresh();
	}
	else
	{
		temp_beacon = new Beacon(event.m_x, event.m_y,(float)(DefaultConfs::getDEFAULT_BEACON_RADIUS() * maze_canvas->getZoomFactor()));
	}
}

void BeaconTool::resetTool(void)
{
	if (temp_beacon != NULL)
	{
		MazeObject * temp = temp_beacon;
		temp_beacon = NULL;
		delete temp;
	}
}

void BeaconTool::mouseRightButtonDown(wxMouseEvent & event)
{
	maze_canvas->setActiveTool(ARROW_TOOL);
}


void BeaconTool::keyDown(wxKeyEvent & event)
{

}

void BeaconTool::mouseLeftDoubleClick(wxMouseEvent & event)
{
}




