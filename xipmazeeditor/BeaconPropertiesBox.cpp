#include "BeaconPropertiesBox.h"

BEGIN_EVENT_TABLE(BeaconPropertiesBox, wxDialog)
	EVT_BUTTON(7, BeaconPropertiesBox::onClickOkButton) 
	EVT_BUTTON(8, BeaconPropertiesBox::onClickCancelButton) 
END_EVENT_TABLE()



BeaconPropertiesBox::BeaconPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas) :
wxDialog((wxWindow *)parent, -1, wxString("Beacon and Target Properties"), wxDefaultPosition, wxSize(300,230)), 
maze_canvas(maze_canvas)
{
	this->beacon_height = NULL;
	this->target_radius = NULL; 
	this->target_center_x = NULL;
	this->target_center_y = NULL;
	this->beacon_center_x = NULL;
	this->beacon_center_y =NULL;

	int offset_x = 20;

	text_beacon_height = new wxTextCtrl(this, 1, wxString(""), wxPoint(offset_x+120,10));
	text_beacon_center_x = new wxTextCtrl(this, 2, wxString(""), wxPoint(offset_x+120,35));
	text_beacon_center_y = new wxTextCtrl(this, 3, wxString(""), wxPoint(offset_x+120,60));
	text_target_radius = new wxTextCtrl(this, 4, wxString(""), wxPoint(offset_x+120,85));
	text_target_center_x = new wxTextCtrl(this, 5, wxString(""), wxPoint(offset_x+120,110));
	text_target_center_y = new wxTextCtrl(this, 6, wxString(""), wxPoint(offset_x+120,135));


	new wxStaticText(this, -1, wxString("Beacon Height:"), wxPoint(offset_x+20,10));
	new wxStaticText(this, -1, wxString("Beacon Center X:"), wxPoint(offset_x+20,35));
	new wxStaticText(this, -1, wxString("Beacon Center Y:"), wxPoint(offset_x+20,60));
	new wxStaticText(this, -1, wxString("Target Radius:"), wxPoint(offset_x+20,85));
	new wxStaticText(this, -1, wxString("Target Center X:"), wxPoint(offset_x+20,110));
	new wxStaticText(this, -1, wxString("Target Center Y:"), wxPoint(offset_x+20,135));

	new wxButton(this, 7, wxString("Ok"), wxPoint(offset_x+30,165));
	new wxButton(this, 8, wxString("Cancel"), wxPoint(offset_x+135, 165));
}


void BeaconPropertiesBox::showBeaconPropertiesBox(float *beacon_height, int *target_radius, 
		int *target_center_x, int *target_center_y,
		int *beacon_center_x, int *beacon_center_y)
{
	this->beacon_height = beacon_height;
	this->target_radius = target_radius; 
	this->target_center_x = target_center_x;
	this->target_center_y = target_center_y;
	this->beacon_center_x = beacon_center_x;
	this->beacon_center_y = beacon_center_y;

	wxString temp;

	int height = maze_canvas->getWindowCurrentHeight();

	temp.Empty();
	temp.Printf("%f", *beacon_height);
	text_beacon_height->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)*beacon_center_x) / maze_canvas->getZoomFactor());
	text_beacon_center_x->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)(height-*beacon_center_y)) / maze_canvas->getZoomFactor());
	text_beacon_center_y->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)*target_center_x) / maze_canvas->getZoomFactor());
	text_target_center_x->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)(height-*target_center_y)) / maze_canvas->getZoomFactor());
	text_target_center_y->SetValue(temp);

	temp.Empty();
	temp.Printf("%f", ((float)*target_radius) / maze_canvas->getZoomFactor());
	text_target_radius->SetValue(temp);

	Center();
	ShowModal();
}



void BeaconPropertiesBox::onClickOkButton(wxCommandEvent &event)
{
	int height = maze_canvas->getWindowCurrentHeight();
	sscanf((const char *)(text_beacon_height->GetValue()), "%f", beacon_height);

	getIntValue(text_beacon_center_x, beacon_center_x);
	getIntValue(text_beacon_center_y, beacon_center_y);
	*beacon_center_y = height - *beacon_center_y;
	getIntValue(text_target_center_x, target_center_x);
	getIntValue(text_target_center_y, target_center_y);
	*target_center_y = height - *target_center_y;
	getIntValue(text_target_radius, target_radius);

	maze_canvas->forceRefresh();
	EndModal(0);
}

void BeaconPropertiesBox::onClickCancelButton(wxCommandEvent &event)
{
	EndModal(0);
}

void BeaconPropertiesBox::getIntValue(wxTextCtrl * text, int * value)
{
	float temp;
	sscanf((const char *)(text->GetValue()), "%f", &temp);
	*value = (int)(temp * maze_canvas->getZoomFactor());
}

