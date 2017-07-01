#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "wx/gdicmn.h"
#include "wx/dc.h"

#include "Defines.h"

class Polygon
{
public:
    Polygon();
	~Polygon();
    
    bool isClosed();
    void close();
    void addPoint(int x, int y);
    void addPoint(wxPoint * p);    
    bool pointInsidePolygon(int xin, int yin);
    bool pointInsidePolygon(wxPoint * p);
	int getNumberOfVertexes(void);
    
	void paintPolygon(wxDC * dc,  int offset_x, int offset_y);

    static const int POLYGON_DEFAULT_SIZE_LIST;
    
protected:
		
    void doubleSize(void);

    wxPoint * points;
	int maximum_number_points;
	int next_empty_point_position;
	int xoffset, yoffset;
	
    bool closed;
};

#endif


