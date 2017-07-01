#ifndef _MAZEFILEDIALOG_H_
#define _MAZEFILEDIALOG_H_

#include "wx/dialog.h"
#include "wx/stattext.h"
#include "wx/gdicmn.h"
#include "wx/button.h"
#include "wx/filename.h"

#include "Defines.h"
#include "MyFrame.h"



class MazeFileDialog : public wxDialog
{

public:

	MazeFileDialog(
		MyFrame * parent, 
		wxFileName &maze_file_path, 
		wxFileName &starting_grid_file_path);

	~MazeFileDialog();

	int showOpenFileDialog(void);
	int showSaveAsFileDialog(void);

	


protected:

	/** Button events *************************/
	void buttonCancelClicked(wxCommandEvent &event);
	void buttonOkClicked(wxCommandEvent &event);
	void buttonStartingGridFilePathClicked(wxCommandEvent &event);
	void buttonMazeFilePathClicked(wxCommandEvent &event);

	/** MazeDialogControlFunctions ************/
	void setCorrectOkButtonState(void);
	void showFileSelectorDialog(wxFileName &file_name, wxString &title);


	MyFrame * myframe;
	wxFileName &maze_file_path;
	wxFileName &starting_grid_file_path; 	

	wxStaticText * maze_file_path_text;
	wxStaticText * starting_grid_file_path_text;

	wxButton * ok_button;	
	wxButton * cancel_button;

	wxButton * maze_file_path_button;
	wxButton * starting_grid_file_path_button;

	bool save_mode;


	DECLARE_EVENT_TABLE()
};

#endif


