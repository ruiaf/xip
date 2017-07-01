#ifndef _MYFRAME_H_
#define _MYFRAME_H_


#include "wx/image.h"
#include "wx/menu.h"
#include "wx/dialog.h"
#include "wx/toolbar.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/icon.h"
#include "wx/window.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/filedlg.h"
#include "wx/statusbr.h"
#include "wx/slider.h"


#include "Defines.h"
#include "DefaultConfs.h"
#include "MazeCanvas.h"
#include "MazeFileDialog.h"
#include "MazeFileHandler.h"
#include "BeaconPropertiesBox.h"
#include "StartingGridPropertiesBox.h"
#include "WallPropertiesBox.h"
#include "MazeCanvasPropertiesBox.h"
#include "ExportDialog.h"

class MazeCanvas;

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);
	~MyFrame();


    void OnNew(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileExport(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnGrid(wxCommandEvent& event);
	void OnMazeCanvas(wxCommandEvent &event);

	void setActiveToolBarToolButton(int tool_type);
	void enableToolBarTool(int tool_type, bool enable);
	void unpressToolBarButtons(void);

	void saveMazeFiles(void);
	void openMazeFiles(void);

	void setCoordinates(float x, float y);

	static BeaconPropertiesBox * beacon_properties_box;
	static StartingGridPropertiesBox * starting_grid_properties_box;
	static WallPropertiesBox * wall_properties_box;
	static MazeCanvasPropertiesBox * maze_canvas_properties_box;

private:

	void beaconTool(wxCommandEvent & event);
	void arrowTool(wxCommandEvent & event);
	void startingGridTool(wxCommandEvent & event);
	void lowWallTool(wxCommandEvent & event);
	void highWallTool(wxCommandEvent & event);

	void onSize(wxSizeEvent& event);

	void initializeToolbar(void);

	/**** Frame Control variables *****/
    MazeCanvas * maze_canvas;
	MazeFileDialog * maze_file_dialog;
	wxFileName maze_file_path;
	wxFileName starting_grid_file_path; 

	MazeFileHandler * maze_file_handler;
	ExportDialog * export_dialog;

	wxMenuBar * menuBar;
    
    DECLARE_EVENT_TABLE()
};

#endif


