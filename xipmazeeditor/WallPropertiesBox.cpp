#include "WallPropertiesBox.h"

BEGIN_EVENT_TABLE(WallPropertiesBox, wxDialog)
	EVT_BUTTON(7, WallPropertiesBox::onClickOkButton) 
	EVT_BUTTON(8, WallPropertiesBox::onClickCancelButton) 
END_EVENT_TABLE()



WallPropertiesBox::WallPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas) :
wxDialog((wxWindow *)parent, -1, wxString("Wall Properties"), wxDefaultPosition, wxSize(300,120)), 
maze_canvas(maze_canvas)
{
	this->wall_height;

	int offset_x = 20;

	text_wall_height = new wxTextCtrl(this, 1, wxString(""), wxPoint(offset_x+120,10));

	new wxStaticText(this, -1, wxString("Wall Height:"), wxPoint(offset_x+20,10));

	new wxButton(this, 7, wxString("Ok"), wxPoint(offset_x+30,50));
	new wxButton(this, 8, wxString("Cancel"), wxPoint(offset_x+135, 50));
}


void WallPropertiesBox::showWallPropertiesBox(float * wall_height)
{
	this->wall_height = wall_height;

	wxString temp;

	temp.Empty();
	temp.Printf("%f", (float)*wall_height);
	text_wall_height->SetValue(temp);

	
	Center();
	ShowModal();
}



void WallPropertiesBox::onClickOkButton(wxCommandEvent &event)
{
	sscanf((const char *)(text_wall_height->GetValue()), "%f", wall_height);
	
	maze_canvas->forceRefresh();
	EndModal(0);
}

void WallPropertiesBox::onClickCancelButton(wxCommandEvent &event)
{
	EndModal(0);
}

void WallPropertiesBox::getIntValue(wxTextCtrl * text, int * value)
{
	float temp;
	sscanf((const char *)(text->GetValue()), "%f", &temp);
	*value = (int)(temp * maze_canvas->getZoomFactor());
}

