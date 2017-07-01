#include "MazeFileDialog.h"

BEGIN_EVENT_TABLE(MazeFileDialog, wxDialog)
	EVT_BUTTON(cancel_button_id,					MazeFileDialog::buttonCancelClicked)
	EVT_BUTTON(ok_button_id,						MazeFileDialog::buttonOkClicked)
	EVT_BUTTON(starting_grid_file_path_button_id,	MazeFileDialog::buttonStartingGridFilePathClicked)
	EVT_BUTTON(maze_file_path_button_id,			MazeFileDialog::buttonMazeFilePathClicked)
END_EVENT_TABLE()



MazeFileDialog::MazeFileDialog(
	MyFrame * parent,
	wxFileName &maze_file_path,
	wxFileName &starting_grid_file_path)
	:
	wxDialog(parent,-1,wxString(""), wxDefaultPosition, wxSize(400,200)),
	myframe(parent),
	maze_file_path(maze_file_path),
	starting_grid_file_path(starting_grid_file_path)
{
	maze_file_path_text = new wxStaticText(this,-1,wxString("Maze file:"), wxPoint(35,40));
	maze_file_path_button = new wxButton(this, maze_file_path_button_id, wxString("Choose maze file..."), wxPoint(125,36), wxSize(250,20));

	starting_grid_file_path_text = new wxStaticText(this,-1,wxString("Starting grid file:"), wxPoint(35,70));
	starting_grid_file_path_button = new wxButton(this, starting_grid_file_path_button_id, wxString("Choose starting grid file..."), wxPoint(125,66), wxSize(250,20));

	cancel_button = new wxButton(this, cancel_button_id, wxString("Cancel"), wxPoint(250,130));
	ok_button = new wxButton(this, ok_button_id, wxString("Open"), wxPoint(50,130));
}

MazeFileDialog::~MazeFileDialog()
{
	delete starting_grid_file_path_button;
	delete maze_file_path_button;
	delete ok_button;
	delete cancel_button;
	delete starting_grid_file_path_text;
	delete maze_file_path_text;
}

int MazeFileDialog::showOpenFileDialog(void)
{
	save_mode=false;
	ok_button->SetLabel(wxString("Open"));
	setCorrectOkButtonState();
	SetTitle(wxString("Choose the maze files to open"));
	return ShowModal();
}

int MazeFileDialog::showSaveAsFileDialog(void)
{
	save_mode=true;
	ok_button->SetLabel(wxString("Save"));
	setCorrectOkButtonState();
	SetTitle(wxString("Choose the files where to save the maze"));
	return ShowModal();
}

void MazeFileDialog::buttonCancelClicked(wxCommandEvent &event)
{
	this->EndModal(-1);
}

void MazeFileDialog::buttonOkClicked(wxCommandEvent &event)
{
	this->EndModal(0);
	if ( this->save_mode)
		myframe->saveMazeFiles();
	else
		myframe->openMazeFiles();
}

void MazeFileDialog::buttonStartingGridFilePathClicked(wxCommandEvent &event)
{
	wxString title("Choose a starting grid file");
	showFileSelectorDialog(starting_grid_file_path, title);

	if (!starting_grid_file_path.GetFullName().IsEmpty())
		starting_grid_file_path_button->SetLabel(starting_grid_file_path.GetFullName());
	setCorrectOkButtonState();
}

void MazeFileDialog::buttonMazeFilePathClicked(wxCommandEvent &event)
{
	wxString title("Choose a maze file");
	showFileSelectorDialog(maze_file_path, title);

	if (!maze_file_path.GetFullName().IsEmpty())
		maze_file_path_button->SetLabel(maze_file_path.GetFullName());
	setCorrectOkButtonState();
}

void MazeFileDialog::setCorrectOkButtonState(void)
{
	if (starting_grid_file_path.HasName() && maze_file_path.HasName() &&
		(save_mode ||
		!save_mode && maze_file_path.FileExists() && starting_grid_file_path.FileExists()))
		ok_button->Enable();
	else
		ok_button->Disable();
}

void MazeFileDialog::showFileSelectorDialog(wxFileName &file_name, wxString &title)
{
	int flags;
	if (save_mode)
	{
		title += " to save";
		flags = wxFD_SAVE|wxFD_OVERWRITE_PROMPT;
	}
	else
	{
		title += " to open";
		flags = wxFD_FILE_MUST_EXIST|wxFD_OPEN;
	}

	wxString temp;
	temp =::wxFileSelector(
					title,
					file_name.GetPath(),
					file_name.GetName(),
					file_name.GetExt(),
					"XML files (*.xml)|*.xml",
					flags,
					this);
	if ( !temp.IsEmpty() )
		file_name = temp;
}


