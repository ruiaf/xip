#ifndef _MAZEOBJECT_H_
#define _MAZEOBJECT_H_

#include "wx/dc.h"
#include "wx/event.h"
#include "wx/file.h"

class MazeObject : public wxObject
{
public:
	MazeObject(){ selected = false; }

	virtual void paint(wxDC * dc, int offset_x, int offset_y) = 0;
	virtual void setPosition(int x, int y) = 0;
	virtual void translateBy(int x, int y) = 0;
	virtual bool isHited(int x, int y) = 0;
	virtual int getType(void)=0;
	virtual bool valid(void)=0;
	virtual bool mazeObjectInsideRectangle(int off_x, int off_y, int width, int height)=0;
	virtual void showPropertiesBox()=0;
	virtual void setSelected(bool value) { selected = value; }
	virtual void doMirrorOnY(int height)=0;

	// return number of written bytes or -1 on error;
	virtual int toXmlFile(wxFile & file_to_write_in, MazeCanvas * maze_canvas)=0;

protected:
	bool selected;
	
};

#endif


