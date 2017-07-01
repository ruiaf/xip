#ifndef _WALL_H_
#define _WALL_H_

#include "Defines.h"
#include "DefaultConfs.h"
#include "MazeObject.h"
#include "Polygon.h"
#include "MyFrame.h"


#include "wx/event.h"

class Wall : public MazeObject, public Polygon
{
public:
	
	Wall(float height = 1.0);
	~Wall();

	
	void paint(wxDC * dc, int offset_x, int offset_y);
	void setPosition(int x, int y);
	void setNextVertexPosition(int x, int y);
	void translateBy(int x, int y);
	bool isHited(int x, int y);
	bool valid(void);
	bool inFirstVertexArea(int x, int y);
	bool mazeObjectInsideRectangle(int off_x, int off_y, int width, int height);
	void showPropertiesBox();
	void doMirrorOnY(int height);

	int toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas);


	int getType(void);

private:

	/**********************************/
	/*** Wall control variables *****/
	
	float wall_height;
	bool mouse_over_first_vertex;	

	/* Constant Wall variables */

	static const wxBrush HIGH_WALLS_BRUSH;
	static const wxPen HIGH_WALLS_PEN;
	
	static const wxBrush LOW_WALLS_BRUSH;	
	static const wxPen LOW_WALLS_PEN;

	static const wxBrush SELECTED_WALLS_BRUSH;	
	static const wxPen SELECTED_WALLS_PEN;

};

#endif

