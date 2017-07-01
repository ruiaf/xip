#ifndef _BEACON_H_
#define _BEACON_H_

#include "wx/event.h"


#include "Defines.h"
#include "MazeObject.h"
#include "DefaultConfs.h"
#include "BeaconPropertiesBox.h"
#include "MyFrame.h"


class Beacon : public MazeObject
{
public:

	Beacon();
	Beacon(int cx, int cy, float radius, float height = (float)DefaultConfs::getDEFAULT_BEACON_HEIGHT());

	void paint(wxDC * dc, int offset_x, int offset_y);
	void getBeaconPosition(int &posx, int &posy);
	void setPosition(int x, int y);
	void setBeaconPosition(int x, int y);
	void setTargetPosition(int x, int y);
	void setHeight(float new_height);
	void setRadius(float new_radius);
	void translateBy(int x, int y);
	bool isHited(int x, int y);
	bool valid(void);
	bool mazeObjectInsideRectangle(int off_x, int off_y, int width, int height);
	void showPropertiesBox();
	void doMirrorOnY(int height);

	int getType(void);

	int toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas);

private:

	/**********************************/
	/*** Beacon control variables *****/

	int beacon_center_x;
	int beacon_center_y;
	int target_center_x;
	int target_center_y;
	int target_radius;
	float beacon_height;

	/* Constant Beacon variables */

	static const wxPen BEACON_PEN;
	static const wxBrush BEACON_BRUSH;

	static const wxPen TARGET_PEN;
	static const wxBrush TARGET_BRUSH;

	static const wxPen SELECTED_BEACON_PEN;
	static const wxBrush SELECTED_BEACON_BRUSH;

	static const wxPen SELECTED_TARGET_PEN;
	static const wxBrush SELECTED_TARGET_BRUSH;
};

#endif
