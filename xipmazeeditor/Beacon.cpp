#include "Beacon.h"

const wxPen Beacon::BEACON_PEN(wxColour(0,150,150),1,wxSOLID);
const wxBrush Beacon::BEACON_BRUSH(wxColour(0,200,200),wxSOLID);

const wxPen Beacon::TARGET_PEN(wxColour(150,0,0),1,wxSOLID);
const wxBrush Beacon::TARGET_BRUSH(wxColour(255,100,0),wxSOLID);


const wxPen Beacon::SELECTED_BEACON_PEN(wxColour(170,170,0),1,wxSOLID);
const wxBrush Beacon::SELECTED_BEACON_BRUSH(wxColour(255,255,0),wxSOLID);

const wxPen Beacon::SELECTED_TARGET_PEN(wxColour(170,170,40),1,wxSOLID);
const wxBrush Beacon::SELECTED_TARGET_BRUSH(wxColour(255,255,40),wxSOLID);


Beacon::Beacon()
{
	beacon_center_x = target_center_x = 0;
	beacon_center_y = target_center_y = 0;
	target_radius = 0;
	beacon_height = 0;
}

Beacon::Beacon(int cx, int cy, float radius, float height)
{
	beacon_center_x = target_center_x = cx;
	beacon_center_y = target_center_y = cy;
	target_radius = (int)radius;
	beacon_height = height;
}

void Beacon::paint(wxDC * dc,  int offset_x, int offset_y)
{
	if (selected)
	{
		dc->SetPen(SELECTED_BEACON_PEN);
		dc->SetBrush(SELECTED_BEACON_BRUSH);
	}
	else
	{
		dc->SetPen(BEACON_PEN);
		dc->SetBrush(BEACON_BRUSH);
	}
	
	dc->DrawCircle((wxCoord )target_center_x+offset_x, (wxCoord )target_center_y+offset_y, (wxCoord )target_radius);

	if (selected)
	{
		dc->SetPen(SELECTED_TARGET_PEN);
		dc->SetBrush(SELECTED_TARGET_BRUSH);
	}
	else
	{
		dc->SetPen(TARGET_PEN);
		dc->SetBrush(TARGET_BRUSH);
	}

	dc->DrawCircle((wxCoord )beacon_center_x+offset_x, (wxCoord )beacon_center_y+offset_y, (wxCoord )(target_radius*0.25));
}

void Beacon::setPosition(int x, int y)
{
	beacon_center_x -= target_center_x - x;
	beacon_center_y -= target_center_y - y;
	target_center_x = x;
	target_center_y = y;
}

bool Beacon::isHited(int x, int y)
{
	return ( target_radius*target_radius >= 
		(target_center_x-x)*(target_center_x-x) + 
		(target_center_y-y)*(target_center_y-y)  ) || 
		( target_radius*target_radius*0.25*0.25 >= 
		(beacon_center_x-x)*(beacon_center_x-x) + 
		(beacon_center_y-y)*(beacon_center_y-y)  );
}

int Beacon::getType(void)
{
	return TYPE_BEACON;
}

bool Beacon::valid(void)
{
	return true;
}

void Beacon::translateBy(int x, int y)
{
	target_center_x += x;
	target_center_y += y;
	beacon_center_x += x;
	beacon_center_y += y;
}


bool Beacon::mazeObjectInsideRectangle(int off_x, int off_y, int width, int height)
{
	return beacon_center_x-off_x-(int)target_radius>=0 && beacon_center_x-off_x+(int)target_radius<=width &&
		   beacon_center_y-off_y-(int)target_radius>=0 && beacon_center_y-off_y+(int)target_radius<=height &&
		   target_center_x-off_x-(int)target_radius>=0 && target_center_x-off_x+(int)target_radius<=width &&
		   target_center_y-off_y-(int)target_radius>=0 && target_center_y-off_y+(int)target_radius<=height;
}

void Beacon::setHeight(float new_height)
{
	beacon_height = new_height;
}

void Beacon::setRadius(float new_radius)
{
	target_radius = (int)new_radius;
}


int Beacon::toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas)
{
	int written_bytes = 0;
	char buffer[1024];

	sprintf(buffer, "\t<Beacon X=\"%lf\" Y=\"%lf\" Height=\"%lf\" />\n", (double)beacon_center_x/maze_canvas->getZoomFactor(), (double)beacon_center_y/maze_canvas->getZoomFactor() , (double)beacon_height);
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));


	sprintf(buffer, "\t<Target X=\"%lf\" Y=\"%lf\" Radius=\"%lf\" />\n\n", (double)target_center_x/maze_canvas->getZoomFactor(), (double)target_center_y/maze_canvas->getZoomFactor(), (double)target_radius/maze_canvas->getZoomFactor());
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));


	return written_bytes;
}

void Beacon::showPropertiesBox()
{
	if (MyFrame::beacon_properties_box != NULL)
		MyFrame::beacon_properties_box->showBeaconPropertiesBox(&beacon_height, &target_radius,
				&target_center_x, &target_center_y, &beacon_center_x, &beacon_center_y);
}


void Beacon::doMirrorOnY(int height)
{
	if ( height >= beacon_center_y)
		beacon_center_y = height - beacon_center_y;
	if ( height >= target_center_y)
		target_center_y = height - target_center_y;
}


void Beacon::setBeaconPosition(int x, int y)
{
	beacon_center_x = x;
	beacon_center_y = y;
}

void Beacon::setTargetPosition(int x, int y)
{
	target_center_x = x;
	target_center_y = y;
}


void Beacon::getBeaconPosition(int &posx, int &posy)
{
	posx = beacon_center_x;
	posy = beacon_center_y;
}

