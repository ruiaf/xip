#include "Wall.h"

const wxBrush Wall::HIGH_WALLS_BRUSH(wxColour(0,255,0),wxSOLID);
const wxPen Wall::HIGH_WALLS_PEN(wxColour(0,200,0),1,wxSOLID);


const wxBrush Wall::LOW_WALLS_BRUSH(wxColour(0,0,255),wxSOLID);
const wxPen Wall::LOW_WALLS_PEN(wxColour(0,0,200),1,wxSOLID);


const wxBrush Wall::SELECTED_WALLS_BRUSH(wxColour(255,255,0),wxSOLID);
const wxPen Wall::SELECTED_WALLS_PEN(wxColour(170,170,0),1,wxSOLID);



Wall::Wall(float height )
{
	mouse_over_first_vertex = false;
	wall_height = height;
	setPosition(0,0);
	
}

Wall::~Wall()
{

}

void Wall::paint(wxDC * dc, int offset_x, int offset_y)
{
	if (selected)
	{
		dc->SetBrush(SELECTED_WALLS_BRUSH);
		dc->SetPen(SELECTED_WALLS_PEN);
	}
	else if (wall_height>DefaultConfs::getMAXIMUM_LOW_WALL_HEIGHT())
	{
		dc->SetBrush(HIGH_WALLS_BRUSH);
		dc->SetPen(HIGH_WALLS_PEN);
	}
	else
	{
		dc->SetBrush(LOW_WALLS_BRUSH);
		dc->SetPen(LOW_WALLS_PEN);
	}
	paintPolygon(dc,offset_x, offset_y);

	if(valid() && !isClosed() && mouse_over_first_vertex)
	{		
		dc->SetBrush(*wxTRANSPARENT_BRUSH);
		dc->DrawCircle(offset_x+points[0].x,offset_y+points[0].y,5);
	}
}

void Wall::setPosition(int x, int y)
{
	xoffset = x;
	yoffset = y;
}

bool Wall::isHited(int x, int y)
{
	return pointInsidePolygon(x,y);
}


int Wall::getType(void)
{
	if (wall_height > DefaultConfs::getMAXIMUM_LOW_WALL_HEIGHT())
		return TYPE_HIGH_WALL;
	else
		return TYPE_LOW_WALL;
}

bool Wall::valid(void)
{
	return next_empty_point_position > 3;
}


bool Wall::inFirstVertexArea(int x, int y)
{
	if ( next_empty_point_position > 2 &&
		 abs(x-points[0].x) <= 5 &&
		 abs(y-points[0].y) <= 5)
	{
		mouse_over_first_vertex=true;
		 return true;
	}

	mouse_over_first_vertex=false;
	return false;
}

void Wall::translateBy(int x, int y)
{
	xoffset += x;
	yoffset += y;
}

void Wall::setNextVertexPosition(int x, int y)
{
	if (next_empty_point_position >= maximum_number_points)
		doubleSize();

	points[next_empty_point_position].x = x;
	points[next_empty_point_position].y = y;
}

bool Wall::mazeObjectInsideRectangle(int off_x, int off_y, int width, int height)
{
	for(int n = 0 ; n < getNumberOfVertexes() ; n++)
		if ( !( points[n].x+xoffset-off_x>=0      &&
				points[n].x+xoffset-off_x<=width  &&
				points[n].y+yoffset-off_y>=0      &&
				points[n].y+yoffset-off_y<=height ))
				return false;

	return true;
}


int Wall::toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas)
{
	int written_bytes = 0;
	char buffer[1024];

	sprintf(buffer, "\t<Wall Height=\"%lf\">\n", wall_height);
	written_bytes = file_to_write_in.Write(buffer, strlen(buffer));

	for ( int x = 0, nvertexes = getNumberOfVertexes(); x < nvertexes ; x++)
	{
		sprintf(buffer, "\t\t<Corner X=\"%lf\" Y=\"%lf\" />\n",  (double)(points[x].x+xoffset)/maze_canvas->getZoomFactor(), (double)(points[x].y+yoffset)/maze_canvas->getZoomFactor());
		written_bytes = file_to_write_in.Write(buffer, strlen(buffer));
	}

	sprintf(buffer, "\t</Wall>\n\n");
	written_bytes = file_to_write_in.Write(buffer, strlen(buffer));


	return written_bytes;
}


void Wall::showPropertiesBox()
{
	if (MyFrame::wall_properties_box != NULL)
		MyFrame::wall_properties_box->showWallPropertiesBox(&wall_height);
}

void Wall::doMirrorOnY(int height)
{
	for (int x = 0 ; x < getNumberOfVertexes() ; x++)
	{
		points[x].x = xoffset + points[x].x;
		points[x].y = height - points[x].y - yoffset;
	}
	xoffset = yoffset = 0;
}




