#include "MyFrame.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(wxID_NEW,		MyFrame::OnNew)
    EVT_MENU(wxID_EXIT,		MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT,	MyFrame::OnAbout)
	EVT_MENU(wxID_OPEN ,	MyFrame::OnFileOpen)
	EVT_MENU(wxID_SAVE ,	MyFrame::OnFileExport)
	EVT_MENU(wxID_SAVEAS ,	MyFrame::OnFileSaveAs)
	EVT_MENU(33	,			MyFrame::OnGrid)
	EVT_MENU(34	,			MyFrame::OnMazeCanvas)
	EVT_SIZE( MyFrame::onSize)
	EVT_TOOL(TOOLBARTOOL_ID(BEACON_TOOL), MyFrame::beaconTool)
	EVT_TOOL(TOOLBARTOOL_ID(ARROW_TOOL), MyFrame::arrowTool)
	EVT_TOOL(TOOLBARTOOL_ID(HIGH_WALL_TOOL), MyFrame::highWallTool)
	EVT_TOOL(TOOLBARTOOL_ID(LOW_WALL_TOOL), MyFrame::lowWallTool)
	EVT_TOOL(TOOLBARTOOL_ID(STARTING_GRID_TOOL), MyFrame::startingGridTool)
END_EVENT_TABLE()


BeaconPropertiesBox * MyFrame::beacon_properties_box = NULL;
StartingGridPropertiesBox * MyFrame::starting_grid_properties_box = NULL;
WallPropertiesBox * MyFrame::wall_properties_box = NULL;
MazeCanvasPropertiesBox * MyFrame::maze_canvas_properties_box = NULL;


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style), maze_canvas(NULL), export_dialog(NULL)
{

	DefaultConfs::loadDefaultConfsFromFile();
	wxBitmap * i = new wxBitmap();
	i->LoadFile(wxString(IMAGE_WINDOW_ICON),wxBITMAP_TYPE_BMP );
	wxIcon * ico = new wxIcon();
	ico->CopyFromBitmap(*i);
	this->SetIcon(*ico);
	delete ico;
	delete i;

    wxMenu *menuFile = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
	wxMenu *view = new wxMenu;

	view->AppendCheckItem(33, _T("&Grid...\tCtrl-G"), _T("Turn grid on/off"));
	view->Check(33, true);
	view->Append(34, _T("&Maze Properties\tCtrl-M"), _T("View Maze Properties"));
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));
	menuFile->Append(wxID_NEW, _T("&New maze\tCtrl-N"), _T("New maze"));
	menuFile->Append(wxID_OPEN, _T("&Open maze...\tCtrl-O"), _T("Open maze..."));
	menuFile->InsertSeparator(1);
	menuFile->InsertSeparator(3);
	menuFile->Append(wxID_SAVEAS, _T("&Save maze as...\tCtrl-S"), _T("Save maze as..."));
	menuFile->Append(wxID_SAVE, _T("&Export world state\tCtrl-E"), _T("Export world state"));
	menuFile->InsertSeparator(6);
	menuFile->Append(wxID_EXIT, _T("E&xit\tCtrl-Q"), _T("Quit this program"));

    menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
	menuBar->Append(view, _T("&View"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);


    CreateStatusBar(2);
    SetStatusText(wxString("Welcome to Xip Maze Editor!"));

	initializeToolbar();

    maze_canvas = new MazeCanvas(this,-1,DefaultConfs::getDEFAULT_CANVAS_WIDTH(), DefaultConfs::getDEFAULT_CANVAS_HEIGHT());
    maze_canvas->SetBackgroundColour(wxColour("BLACK"));

	maze_canvas->forceRefresh();

	maze_file_dialog = new MazeFileDialog(this, maze_file_path, starting_grid_file_path);

	beacon_properties_box = new BeaconPropertiesBox(this, maze_canvas);
	starting_grid_properties_box = new StartingGridPropertiesBox(this, maze_canvas);
	wall_properties_box = new WallPropertiesBox(this, maze_canvas);
	maze_canvas_properties_box = new MazeCanvasPropertiesBox(this, maze_canvas);

	this->ShowFullScreen(true,0);
	this->Maximize(true);
	setCoordinates(0,0);

	maze_file_handler = new MazeFileHandler(maze_canvas, &maze_file_path, &starting_grid_file_path);
	export_dialog = new ExportDialog(this, maze_canvas);
}


MyFrame::~MyFrame()
{
	if (beacon_properties_box != NULL)
		delete beacon_properties_box;
	delete maze_file_handler;
	delete maze_file_dialog;
	delete maze_canvas;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf("XIP Maze Editor is brought to you by:\n Nuno Cerqueira\n Rui Ferreira\n Rui Martins");

    wxMessageBox(msg, _T("About"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnFileOpen(wxCommandEvent& event)
{
	maze_file_dialog->Center();
	maze_file_dialog->showOpenFileDialog();
}

void MyFrame::OnFileExport(wxCommandEvent& event)
{
	wxFileName filename;

	wxString temp;
	filename="";
	temp =::wxFileSelector(
					wxString("Choose file to export world state"),
					filename.GetPath(),
					filename.GetName(),
					filename.GetExt(),
					"WST files (*.wst)|*.wst",
					wxFD_SAVE|wxFD_OVERWRITE_PROMPT, this);

	if ( temp.IsEmpty() )
		return;

	filename = temp;

	wxFile file(filename.GetFullPath(), wxFile::write );
	if (!file.IsOpened())
	{
		wxMessageDialog dialog_error(this,
			wxString("Error trying to export world state. Couldn't open choosen file in write mode"),
			wxString("Error"),
			wxICON_ERROR);
		dialog_error.Center();
		dialog_error.ShowModal();
		return;
	}

	if ( -1 == maze_canvas->exportWorldState(file, DefaultConfs::getEXPORT_RESOLUTION()))
	{
		wxMessageDialog dialog_error(this,
			wxString("Error trying to export world state. Please check if the\nchosen starting grid position and a beacon exist"),
			wxString("Error"),
			wxICON_ERROR);
		dialog_error.Center();
		dialog_error.ShowModal();
	}
}

void MyFrame::OnFileSaveAs(wxCommandEvent& event)
{
	maze_file_dialog->Center();
	maze_file_dialog->showSaveAsFileDialog();
}


void MyFrame::initializeToolbar(void)
{

	CreateToolBar();

	this->GetToolBar()->SetToolBitmapSize(wxSize(40,40));

	wxBitmap * fine = new wxBitmap();
	fine->LoadFile(wxString(IMAGE_TOOLBAR_ARROW),wxBITMAP_TYPE_BMP );
	GetToolBar()->AddTool(TOOLBARTOOL_ID(ARROW_TOOL), wxString("Selection Tool"),
		*fine, *fine,wxITEM_CHECK,wxString("Selection Tool"),wxString("Selection Tool"));

	fine = new wxBitmap();
	fine->LoadFile(wxString(IMAGE_TOOLBAR_BEACON),wxBITMAP_TYPE_BMP );
	GetToolBar()->AddTool(TOOLBARTOOL_ID(BEACON_TOOL), wxString("Add Beacon"),
		*fine, *fine,wxITEM_CHECK,wxString("Add Beacon"),wxString("Add Beacon"));


	fine = new wxBitmap();
	fine->LoadFile(wxString(IMAGE_TOOLBAR_STARTING_GRID),wxBITMAP_TYPE_BMP );
	GetToolBar()->AddTool(TOOLBARTOOL_ID(STARTING_GRID_TOOL), wxString("Add Starting Grid Position"),
		*fine, *fine,wxITEM_CHECK,wxString("Add Starting Grid Position"),wxString("Add Starting Grid Position"));


	fine = new wxBitmap();
	fine->LoadFile(wxString(IMAGE_TOOLBAR_HIGH_WALL),wxBITMAP_TYPE_BMP );
	GetToolBar()->AddTool(TOOLBARTOOL_ID(HIGH_WALL_TOOL), wxString("Add High Wall"),
		*fine, *fine,wxITEM_CHECK,wxString("Add High Wall"),wxString("Add High Wall"));


	fine = new wxBitmap();
	fine->LoadFile(wxString(IMAGE_TOOLBAR_LOW_WALL),wxBITMAP_TYPE_BMP );
	GetToolBar()->AddTool(TOOLBARTOOL_ID(LOW_WALL_TOOL), wxString("Add Low Wall"),
		*fine, *fine,wxITEM_CHECK,wxString("Add Low Wall"),wxString("Add Low Wall"));

	GetToolBar()->Realize();
}

void MyFrame::beaconTool(wxCommandEvent & event)
{
	unpressToolBarButtons();
	GetToolBar()->ToggleTool(TOOLBARTOOL_ID(BEACON_TOOL),true);
	maze_canvas->setActiveTool(BEACON_TOOL);
}

void MyFrame::arrowTool(wxCommandEvent & event)
{
	unpressToolBarButtons();
	GetToolBar()->ToggleTool(TOOLBARTOOL_ID(ARROW_TOOL),true);
	maze_canvas->setActiveTool(ARROW_TOOL);
}

void MyFrame::startingGridTool(wxCommandEvent & event)
{
	unpressToolBarButtons();
	GetToolBar()->ToggleTool(TOOLBARTOOL_ID(STARTING_GRID_TOOL),true);
	maze_canvas->setActiveTool(STARTING_GRID_TOOL);
}

void MyFrame::lowWallTool(wxCommandEvent & event)
{
	unpressToolBarButtons();
	GetToolBar()->ToggleTool(TOOLBARTOOL_ID(LOW_WALL_TOOL),true);
	maze_canvas->setActiveTool(LOW_WALL_TOOL);
}

void MyFrame::highWallTool(wxCommandEvent & event)
{
	unpressToolBarButtons();
	GetToolBar()->ToggleTool(TOOLBARTOOL_ID(HIGH_WALL_TOOL),true);
	maze_canvas->setActiveTool(HIGH_WALL_TOOL);
}


void MyFrame::unpressToolBarButtons(void)
{
	for (int x = TOOLBARTOOL_ID_OFFSET ; x < TOOL_QUANTITY+TOOLBARTOOL_ID_OFFSET ; x++)
		GetToolBar()->ToggleTool(x,false);
}

void MyFrame::setActiveToolBarToolButton(int tool_type)
{
	if (tool_type==NO_TOOL)
	{
		unpressToolBarButtons();
	}
	else if (!GetToolBar()->GetToolState(TOOLBARTOOL_ID(tool_type)))
	{
		unpressToolBarButtons();
		GetToolBar()->ToggleTool(TOOLBARTOOL_ID(tool_type ),true);
	}
}

void MyFrame::openMazeFiles(void)
{
	maze_canvas->clearAllCanvas();
	maze_file_handler->openAll();
}

void MyFrame::saveMazeFiles(void)
{
	maze_file_handler->saveAll();
}


void MyFrame::setCoordinates(float x, float y)
{
	wxString temp;
	temp.Printf("(%.1f, %.1f)",x,y);
	SetStatusText(temp,1);
}


void MyFrame::enableToolBarTool(int tool_type, bool enable)
{
	GetToolBar()->EnableTool(TOOLBARTOOL_ID(tool_type), enable);
}


void MyFrame::onSize(wxSizeEvent& event)
{
	if (maze_canvas != NULL)
	{
		maze_canvas->adjustCanvas();
	}
}

void MyFrame::OnNew(wxCommandEvent& event)
{
	if (maze_canvas != NULL)
	{
		maze_canvas->clearAllCanvas();
	}
}


void MyFrame::OnGrid(wxCommandEvent& event)
{
	maze_canvas->setGridEnabled(event.IsChecked());
}

void MyFrame::OnMazeCanvas(wxCommandEvent &event)
{
	if(maze_canvas != NULL)
		maze_canvas->showPropertiesBox();
}



