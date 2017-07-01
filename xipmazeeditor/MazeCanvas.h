#ifndef _MAZECANVAS_H_
#define _MAZECANVAS_H_


#include "wx/window.h"
#include "wx/event.h"
#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/dialog.h"
#include "wx/dc.h"
#include "wx/pen.h"
#include "wx/colour.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/scrolwin.h"

#include <list>

#include "Defines.h"

#include "ArrowTool.h"
#include "Beacon.h"
#include "BeaconTool.h"
#include "DefaultConfs.h"
#include "MazeObject.h"
#include "MazeTool.h"
#include "MyFrame.h"
#include "StartingGridTool.h"
#include "Wall.h"
#include "WallTool.h"
#include "MazeCanvasPropertiesBox.h"


class MazeCanvas : public wxWindow
{
public:
    MazeCanvas( MyFrame * my_frame, wxWindowID id, double new_canvas_width, double new_canvas_height);
	~MazeCanvas();

	/*********************************************/
	int toXmlGridFile(wxFile & file_to_write_in);
	int toXmlMazeFile(wxFile & file_to_write_in);

	int exportWorldState(wxFile & file_to_write_in, double resolution);


	void addNewObject(MazeObject * mo);
	void forceRefresh(void);

	void verifyConstraints(void);

	void setActiveTool(int tool);
	void enableTool(int tool, bool enable);

	void setCanvasSize(double new_canvas_width, double new_canvas_height);

	Beacon * getBeacon(void);
	StartingGrid * getStartingGrid(int grid_position);

	MazeObject * getSelectedObject(int x, int y);
	void clearAllCanvas(void);

	void setZoomFactor(double new_zoom_factor);
	double getZoomFactor(void);

	void setTopLeftOffsets(int x, int y);

	void adjustCanvas(void);

	int getWindowCurrentWidth(void);
	int getWindowCurrentHeight(void);

	void doMirrorOnY();

	int getFirstStartingGridUnusedNumber();

	void setGridEnabled(bool enable);

	void showPropertiesBox();


protected:
private:

	/*********************************************/
	/*** Auxiliary functions *********************/


	bool isInsideCanvas(int x, int y);
	void snapToGrid(int &x, int &y);
	void getNumberMazeObjects(int &nbeacons, int &nstartgrids, int &nwalls);

	/*********************************************/
	/*** Event handling functions ****************/

    void mouseLeftButtonDown(wxMouseEvent & event);
	void mouseLeftButtonUp(wxMouseEvent & event);
    void mouseRightButtonDown(wxMouseEvent & event);
	void mouseLeftDoubleClick(wxMouseEvent & event);
    void mouseMoved(wxMouseEvent & event);
	void mouseLeftWindow(wxMouseEvent & event);
    void onPaint(wxPaintEvent & event);
    void onErase(wxEraseEvent  & event);
	void keyDown(wxKeyEvent & event);
	void onSize(wxSizeEvent & event);
	void scrollWinThumbTrack(wxScrollWinEvent &event);
	void scrollWinThumbRelease(wxScrollWinEvent &event);
	void scrollWinTop(wxScrollWinEvent &event);
	void scrollWinBottom(wxScrollWinEvent &event);
	void scrollWinLineUp(wxScrollWinEvent &event);
	void scrollWinLineDown(wxScrollWinEvent &event);
	void scrollWinPageUp(wxScrollWinEvent &event);
	void scrollWinPageDown(wxScrollWinEvent &event);


	/*********************************************/
	/*** Paint and Double Buffering functions ****/

	void paintBackground(wxDC * dc);
	void paintForeground(wxDC * dc);
	void paintGrid(wxDC * dc, int start_x, int start_y, int width, int height);

	void initializeBitmapsMemoryDCs(int width, int height);

	/*********************************************/
	/******* Variables ***************************/

	MyFrame * my_frame;

	MazeTool ** tools;
	int active_tool;

	bool grid_enabled;

	int window_current_width;
	int window_current_height;

	double canvas_current_width;
	double canvas_current_height;

	double zoom_factor;

	int top_left_canvas_corner_x;
	int top_left_canvas_corner_y;

	/* Grid */

	int grid_pixel_increment;

	/* Double buffering variables */
    wxMemoryDC * background_buffer;
    wxBitmap * background_buffer_bmp;

    wxMemoryDC * mixing_buffer;
    wxBitmap * mixing_buffer_bmp;

	/* Maze Objects */
	 std::list<MazeObject *> unselected_objects;


	/********************************************/
	/******* Predifined styles ******************/

	static const wxBrush BACKGROUND_BRUSH;
	static const wxPen BACKGROUND_PEN;

	static const wxBrush IMAGE_BACKGROUND_BRUSH;
	static const wxPen IMAGE_BACKGROUND_PEN;

	static const wxPen GRID_PEN;


    DECLARE_EVENT_TABLE()
};

#endif


