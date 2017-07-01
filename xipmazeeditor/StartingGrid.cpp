#include "StartingGrid.h"

const wxBrush StartingGrid::STARTING_GRID_BRUSH(wxColour(0,0,150),wxSOLID);	
const wxPen StartingGrid::STARTING_GRID_PEN(wxColour(0,0,200),1,wxSOLID);

const wxBrush StartingGrid::SELECTED_STARTING_GRID_BRUSH(wxColour(255,255,0),wxSOLID);	
const wxPen StartingGrid::SELECTED_STARTING_GRID_PEN(wxColour(170,170,0),1,wxSOLID);


const wxBrush StartingGrid::STARTING_GRID_DIR_BRUSH(wxColour(150,0,150),wxSOLID);	
const wxPen StartingGrid::STARTING_GRID_DIR_PEN(wxColour(150,0,150),1,wxSOLID);

const wxBrush StartingGrid::SELECTED_STARTING_GRID_DIR_BRUSH(wxColour(170,170,0),wxSOLID);	
const wxPen StartingGrid::SELECTED_STARTING_GRID_DIR_PEN(wxColour(170,170,0),1,wxSOLID);




StartingGrid::StartingGrid(int number, int x, int y, double direction)
:pos_x(x), pos_y(y)
{
	setNumber(number);
	setDirection(direction);
}

StartingGrid::~StartingGrid()
{
}

void StartingGrid::paint(wxDC * dc, int offset_x, int offset_y)
{
	if (selected)
	{
		dc->SetBrush(SELECTED_STARTING_GRID_BRUSH);
		dc->SetPen(SELECTED_STARTING_GRID_PEN);
	}
	else
	{
		dc->SetBrush(STARTING_GRID_BRUSH);
		dc->SetPen(STARTING_GRID_PEN);
	}
	
	dc->DrawRectangle((int)(offset_x+pos_x-DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2),
		(int)(offset_y+pos_y-DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2), 
		(int)(DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()), 
		(int)(DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()));

	if (selected)
	{
		dc->SetBrush(SELECTED_STARTING_GRID_DIR_BRUSH);
		dc->SetPen(SELECTED_STARTING_GRID_DIR_PEN);
	}
	else
	{
		dc->SetBrush(STARTING_GRID_DIR_BRUSH);
		dc->SetPen(STARTING_GRID_DIR_PEN);
	}

	dc->DrawEllipticArc((int)(offset_x+pos_x-DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2),
		(int)(offset_y+pos_y-DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2), 
		(int)(DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()), 
		(int)(DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()) ,
		direction - 45, 
		direction + 45);

	dc->SetTextForeground(*wxWHITE);
	dc->DrawText(str_number, offset_x+pos_x,(int)(offset_y+pos_y-DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2));
}

void StartingGrid::setPosition(int x, int y)
{
	pos_x = x;
	pos_y = y;
}

bool StartingGrid::isHited(int x, int y)
{	
	return (abs(x-pos_x) <= DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2 &&
		abs(y-pos_y) <= DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()/2);
}


int StartingGrid::getType(void)
{
	return TYPE_STARTING_GRID;
}


bool StartingGrid::valid(void)
{
	return true;
}

void StartingGrid::translateBy(int x, int y)
{
	pos_x += x;
	pos_y += y;
}

bool StartingGrid::mazeObjectInsideRectangle(int off_x, int off_y, int width, int height)
{
	return pos_x-off_x>=0 && pos_x-off_x<=width &&
		   pos_y-off_y>=0 && pos_y-off_y<=height;
}


int StartingGrid::toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas)
{
	int written_bytes=0;
	char buffer[1024];

	sprintf(buffer, "\t<Position X=\"%lf\" Y=\"%lf\" Dir=\"%lf\"/>\n", (double)pos_x/maze_canvas->getZoomFactor(), (double)pos_y/maze_canvas->getZoomFactor(), 360-direction);
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));


	return written_bytes;
}


double StartingGrid::getDirection(void)
{
	return direction;
}

void StartingGrid::setDirection(double new_dir)
{
	while(new_dir >= 360)
		new_dir -= 360;

	direction = new_dir;
}

void StartingGrid::setDirection(double x, double y, double offset_x, double offset_y)
{
	double center_x = offset_x+pos_x;
	double center_y = offset_y+pos_y;
	double vx = x - center_x;
	double vy = y - center_y;

	if (vy == 0 && vx == 0)
		return;

	double inner_product = vx /	sqrt(vx*vx + vy*vy);

	double angle = acos(inner_product) * 180 / PI;

	if ( vy > 0)
		angle = 360 - angle;

	angle = 22.5*(double)((int)( angle/22.5));

	setDirection(angle);
}

void StartingGrid::showPropertiesBox()
{
	if (MyFrame::starting_grid_properties_box != NULL)
		MyFrame::starting_grid_properties_box->showStartingGridPropertiesBox(&direction, &pos_x, &pos_y);
}

void StartingGrid::doMirrorOnY(int height)
{
	if (height > pos_y)
		pos_y = height - pos_y;
}

int StartingGrid::getNumber()
{
	return number;
}

void StartingGrid::setNumber(int new_number)
{
	number = new_number;
	str_number.Empty();
	str_number.Printf("%d", number);
}


void StartingGrid::getPosition(int &x, int &y)
{
	x = pos_x;
	y = pos_y;
}

