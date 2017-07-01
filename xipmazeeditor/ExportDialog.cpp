#include "ExportDialog.h"

#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(ExportDialog, wxDialog)
	EVT_BUTTON(1, ExportDialog::onClickExportButton)
	EVT_BUTTON(2, ExportDialog::onClickCancelButton)
	EVT_BUTTON(3, ExportDialog::onClickChooseFile)
END_EVENT_TABLE()


ExportDialog::ExportDialog(MyFrame * parent, MazeCanvas * maze_canvas) :
wxDialog((wxWindow *) parent, -1, wxString("Export World State"), wxDefaultPosition, wxSize(280,220)),
maze_canvas(maze_canvas)
{
	button_export = new wxButton(this, 1, wxString("Export"), wxPoint(45, 140));
	button_cancel = new wxButton(this, 2, wxString("Cancel"), wxPoint(145, 140));
	button_choose_file = new wxButton(this, 3, wxString("Choose File"), wxPoint(30, 20), wxSize(200, 20));
	radio_1 = new wxRadioButton(this, -1, wxString("Starting Grid 1"), wxPoint(30, 50));
	radio_1->SetValue(true);
	radio_2 = new wxRadioButton(this, -1, wxString("Starting Grid 2"), wxPoint(30, 75));
	radio_3 = new wxRadioButton(this, -1, wxString("Starting Grid 3"), wxPoint(30, 100));
}


void ExportDialog::showExportDialog()
{
	Center();
	ShowModal();
}

void ExportDialog::onClickExportButton(wxCommandEvent &event)
{
	if (filename.GetName().IsEmpty())
	{
		wxMessageDialog dialog_error(this,
			wxString("Error trying to export world state. Please choose a valid file name"),
			wxString("Error"),
			wxICON_ERROR);
		dialog_error.Center();
		dialog_error.ShowModal();
		return;
	}
	wxFile file(filename.GetFullPath(), wxFile::write );
	if (!file.IsOpened())
	{
		return;
	}

	int grid;

	if ( radio_1->GetValue())
		grid = 1;
	else if ( radio_2->GetValue())
		grid = 2;
	else
		grid = 3;

	if ( -1 == maze_canvas->exportWorldState(file, DefaultConfs::getEXPORT_RESOLUTION()))
	{
		wxMessageDialog dialog_error(this,
			wxString("Error trying to export world state. Please check if the\nchosen starting grid position and a beacon exist"),
			wxString("Error"),
			wxICON_ERROR);
		dialog_error.Center();
		dialog_error.ShowModal();
	}
	else
		EndModal(0);
}

void ExportDialog::onClickCancelButton(wxCommandEvent &event)
{
	EndModal(0);
}

void ExportDialog::onClickChooseFile(wxCommandEvent &event)
{
	wxString temp;
	filename="";
	temp =::wxFileSelector(
					wxString("Choose file to export world state"),
					filename.GetPath(),
					filename.GetName(),
					filename.GetExt(),
					"WST files (*.wst)|*.wst",
					wxFD_SAVE|wxFD_OVERWRITE_PROMPT,
					this);

	if ( temp.IsEmpty() )
		return;

	filename = temp;
	button_choose_file->SetLabel(filename.GetFullName());
}
