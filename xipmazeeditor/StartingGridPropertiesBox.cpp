#include "StartingGridPropertiesBox.h"

BEGIN_EVENT_TABLE(StartingGridPropertiesBox, wxDialog)
	EVT_BUTTON(7, StartingGridPropertiesBox::onClickOkButton) 
	EVT_BUTTON(8, StartingGridPropertiesBox::onClickCancelButton) 
END_EVENT_TABLE()



StartingGridPropertiesBox::StartingGridPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas) :
wxDialog((wxWindow *)parent, -1, wxString("Starting Grid Properties"), wxDefaultPosition, wxSize(300,170)), 
maze_canvas(maze_canvas)
{
	this->direction = NULL;
	this->pos_x = NULL;
	this->pos_y = NULL;

	int offset_x = 20;

	text_direction = new wxTextCtrl(this, 1, wxString(""), wxPoint(offset_x+130,10));
	text_pos_x = new wxTextCtrl(this, 2, wxString(""), wxPoint(offset_x+130,35));
	text_pos_y = new wxTextCtrl(this, 3, wxString(""), wxPoint(offset_x+130,60));


	new wxStaticText(this, -1, wxString("Starting Grid Direction:"), wxPoint(offset_x+10,10));
	new wxStaticText(this, -1, wxString("Starting Grid Center X:"), wxPoint(offset_x+10,35));
	new wxStaticText(this, -1, wxString("Starting Grid Center Y:"), wxPoint(offset_x+10,60));

	new wxButton(this, 7, wxString("Ok"), wxPoint(offset_x+30,100));
	new wxButton(this, 8, wxString("Cancel"), wxPoint(offset_x+135, 100));
}


void StartingGridPropertiesBox::showStartingGridPropertiesBox(double * direction, int *pos_x, int *pos_y)
{
	this->direction = direction;
	this->pos_x = pos_x;
	this->pos_y = pos_y;

	wxString temp;

	int height = maze_canvas->getWindowCurrentHeight();

	temp.Empty();
	temp.Printf("%f", (float)*direction);
	text_direction->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)*pos_x) / maze_canvas->getZoomFactor());
	text_pos_x->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)(height-*pos_y)) / maze_canvas->getZoomFactor());
	text_pos_y->SetValue(temp);

	Center();
	ShowModal();
}



void StartingGridPropertiesBox::onClickOkButton(wxCommandEvent &event)
{
	int height = maze_canvas->getWindowCurrentHeight();
	sscanf((const char *)(text_direction->GetValue()), "%lf", direction);

	getIntValue(text_pos_x, pos_x);
	getIntValue(text_pos_y, pos_y);
	*pos_y = height - *pos_y;
	
	maze_canvas->forceRefresh();
	EndModal(0);
}

void StartingGridPropertiesBox::onClickCancelButton(wxCommandEvent &event)
{
	EndModal(0);
}

void StartingGridPropertiesBox::getIntValue(wxTextCtrl * text, int * value)
{
	float temp;
	sscanf((const char *)(text->GetValue()), "%f", &temp);
	*value = (int)(temp * maze_canvas->getZoomFactor());
}

