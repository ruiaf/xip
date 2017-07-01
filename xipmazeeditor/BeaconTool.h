#ifndef _BEACONTOOL_H_
#define _BEACONTOOL_H_


#include "Defines.h"
#include "MazeCanvas.h"
#include "MazeTool.h"
#include "Beacon.h"


class BeaconTool : public MazeTool
{

public:	
	BeaconTool(MazeCanvas * mc);
	~BeaconTool();

	void paint(wxDC * dc, int offset_x, int offset_y);

	void mouseLeftButtonDown(wxMouseEvent & event);	
	void mouseLeftButtonUp(wxMouseEvent & event);	
	void mouseLeftDoubleClick(wxMouseEvent & event);	
	void mouseRightButtonDown(wxMouseEvent & event);
	void mouseMoved(wxMouseEvent & event);
	void keyDown(wxKeyEvent & event);

	void resetTool(void);

protected:

	Beacon * temp_beacon;	
};


#endif
