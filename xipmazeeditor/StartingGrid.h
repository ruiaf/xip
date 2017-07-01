#ifndef _STARTINGGRID_H_
#define _STARTINGGRID_H_

#include "math.h"


#include "Defines.h"
#include "DefaultConfs.h"
#include "MazeObject.h"
#include "MyFrame.h"

class StartingGrid : public MazeObject
{
public:

	StartingGrid(int number, int x, int y, double direction = 0);
	~StartingGrid();

	int getNumber();
	void setNumber(int new_number);

	void paint(wxDC * dc, int offset_x, int offset_y);
	void getPosition(int &x, int &y);
	void setPosition(int x, int y) ;
	void translateBy(int x, int y);
	bool isHited(int x, int y) ;
	int getType(void);
	bool valid(void);
	double getDirection(void);
	void setDirection(double new_dir);
	void setDirection(double x, double y, double offset_x, double offset_y);

	bool mazeObjectInsideRectangle(int off_x, int off_y, int width, int height);
	void showPropertiesBox();
	void doMirrorOnY(int height);


	int toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas);

protected:

	int pos_x;
	int pos_y;

	int number;
	wxString str_number;

	double direction;

	static const wxBrush STARTING_GRID_BRUSH;	
	static const wxPen STARTING_GRID_PEN;

	static const wxBrush SELECTED_STARTING_GRID_BRUSH;	
	static const wxPen SELECTED_STARTING_GRID_PEN;

	static const wxBrush STARTING_GRID_DIR_BRUSH;	
	static const wxPen STARTING_GRID_DIR_PEN;

	static const wxBrush SELECTED_STARTING_GRID_DIR_BRUSH;	
	static const wxPen SELECTED_STARTING_GRID_DIR_PEN;
	
};

#endif
