#include "MazeCanvasPropertiesBox.h"

BEGIN_EVENT_TABLE(MazeCanvasPropertiesBox, wxDialog)
	EVT_BUTTON(7, MazeCanvasPropertiesBox::onClickOkButton) 
	EVT_BUTTON(8, MazeCanvasPropertiesBox::onClickCancelButton) 
END_EVENT_TABLE()



MazeCanvasPropertiesBox::MazeCanvasPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas) :
wxDialog((wxWindow *)parent, -1, wxString("Maze Properties"), wxDefaultPosition, wxSize(300,140)), 
maze_canvas(maze_canvas)
{
	this->width = NULL;
	this->height = NULL;

	int offset_x = 20;

	text_width = new wxTextCtrl(this, 1, wxString(""), wxPoint(offset_x+130,10));
	text_height = new wxTextCtrl(this, 2, wxString(""), wxPoint(offset_x+130,35));
	

	new wxStaticText(this, -1, wxString("Maze Width:"), wxPoint(offset_x+10,10));
	new wxStaticText(this, -1, wxString("Maze Height:"), wxPoint(offset_x+10,35));
	
	new wxButton(this, 7, wxString("Ok"), wxPoint(offset_x+30,70));
	new wxButton(this, 8, wxString("Cancel"), wxPoint(offset_x+135, 70));
}


void MazeCanvasPropertiesBox::showMazeCanvasPropertiesBox(double * width, double * height)
{
	this->width = width;
	this->height = height;

	wxString temp;

	temp.Empty();
	temp.Printf("%lf", *width);
	text_width->SetValue(temp);

	temp.Empty();
	temp.Printf("%lf", *height);
	text_height->SetValue(temp);

	Center();
	ShowModal();
}



void MazeCanvasPropertiesBox::onClickOkButton(wxCommandEvent &event)
{
	sscanf((const char *)(text_width->GetValue()), "%lf", width);
	sscanf((const char *)(text_height->GetValue()), "%lf", height);
	
	maze_canvas->forceRefresh();
	EndModal(0);
}

void MazeCanvasPropertiesBox::onClickCancelButton(wxCommandEvent &event)
{
	EndModal(0);
}


