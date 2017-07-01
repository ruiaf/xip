#include "Polygon.h"

const int Polygon::POLYGON_DEFAULT_SIZE_LIST = 64;

/*#include "wx/msgdlg.h"
char buffer[200];
sprintf(buffer,"pos=%d parity=%d", pos, (int)parity);
wxMessageBox(wxString(buffer));*/

Polygon::Polygon()
{
	closed=false;
	xoffset=yoffset=0;
	
	maximum_number_points = POLYGON_DEFAULT_SIZE_LIST;
	next_empty_point_position = 0;

	points = new wxPoint [maximum_number_points];
}

Polygon::~Polygon()
{	
	delete [] points;
}
    
inline bool Polygon::isClosed()
{
    return closed;
}

void Polygon::close()
{
	closed = true;
}


void Polygon::addPoint(int x, int y)
{
	if (isClosed())
		return;
	if ( next_empty_point_position+1 >= maximum_number_points)
		doubleSize();
	
	points[next_empty_point_position].x=x;
	points[next_empty_point_position].y=y;
	next_empty_point_position++;
	points[next_empty_point_position].x=x;
	points[next_empty_point_position].y=y;
}

void Polygon::addPoint(wxPoint * p)
{
	if (isClosed() || p==NULL)
		return;
	addPoint(p->x, p->y);
}
     
int floatCompareForQSort(const void * a, const void * b)
{
	float af = *((float *)a);
	float bf = *((float *)b);
	if (af > bf)
		return 1;
	else if (af < bf )
		return -1;
	else
		return 0;
}


bool Polygon::pointInsidePolygon(int xin, int yin)
{
	/*****************************************/
	/* Algorythm used may be found at:        *
	/* http://www.alienryderflex.com/polygon/ *
	/*****************************************/

	int i, j=0;
	bool oddNODES=false ;
	int polySides = getNumberOfVertexes();

	for (i=0; i<polySides; i++) 
	{
		j++; 
		if (j==polySides) 
			j=0;

		if (points[i].y+yoffset<yin && points[j].y+yoffset>=yin ||
			points[j].y+yoffset<yin && points[i].y+yoffset>=yin) 
		{
				if (points[i].x+((yin-points[i].y-yoffset)*(points[j].x-points[i].x))/(points[j].y-points[i].y) < xin-xoffset) 
				{
					oddNODES=!oddNODES; 
				}
		}
	}

	return oddNODES; 
}

bool Polygon::pointInsidePolygon(wxPoint * p)
{
	return pointInsidePolygon(p->x, p->y);
}

void Polygon::paintPolygon(wxDC * dc, int offset_x, int offset_y)
{
	if (isClosed())
		dc->DrawPolygon(next_empty_point_position, points, offset_x+xoffset, offset_y+yoffset);
	else
	{
		dc->DrawLines(next_empty_point_position+1, points, offset_x, offset_y);
	}
}


void Polygon::doubleSize(void)
{
	maximum_number_points*=2;
	wxPoint * temp = new wxPoint [maximum_number_points];
	for (int x = 0 ; x < next_empty_point_position ; x++)
	{
		temp[x].x = points[x].x;
		temp[x].y = points[x].y;
	}
	
	delete [] points;
	points = temp;
}

int Polygon::getNumberOfVertexes(void)
{
	return next_empty_point_position;
}



