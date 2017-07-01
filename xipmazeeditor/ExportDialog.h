#ifndef _EXPORTDIALOG_H_
#define _EXPORTDIALOG_H_

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/dialog.h"
#include "wx/radiobut.h"
#include "wx/msgdlg.h"


#include "Defines.h"
#include "DefaultConfs.h"
#include "MyFrame.h"
#include "MazeCanvas.h"


class ExportDialog : wxDialog
{
public:
	ExportDialog(MyFrame * parent, MazeCanvas * maze_canvas);
	void showExportDialog();

private:
	void onClickChooseFile(wxCommandEvent &event);
	void onClickExportButton(wxCommandEvent &event);
	void onClickCancelButton(wxCommandEvent &event);

	wxButton * button_export;
	wxButton * button_cancel;
	wxButton * button_choose_file;

	wxRadioButton * radio_1;
	wxRadioButton * radio_2;
	wxRadioButton * radio_3;

	wxFileName filename;
	MazeCanvas * maze_canvas;

	DECLARE_EVENT_TABLE()
};



#endif
