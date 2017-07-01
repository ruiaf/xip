#ifndef _BEACONPROPERTIESBOX_H_
#define _BEACONPROPERTIESBOX_H_

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"


#include "Defines.h"
#include "MazeCanvas.h"


class BeaconPropertiesBox : public wxDialog
{
public:
	BeaconPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas);

	void showBeaconPropertiesBox(float *beacon_height, int *target_radius, 
		int *target_center_x, int *target_center_y,
		int *beacon_center_x, int *beacon_center_y);

protected:
	void onClickOkButton(wxCommandEvent &event); 
	void onClickCancelButton(wxCommandEvent &event);

	void getIntValue(wxTextCtrl * text, int * value);

	MazeCanvas * maze_canvas;

	float *beacon_height;
	int *target_radius; 
	int *target_center_x;
	int *target_center_y;
	int *beacon_center_x;
	int *beacon_center_y;

	wxTextCtrl *text_beacon_height;
	wxTextCtrl *text_target_radius; 
	wxTextCtrl *text_target_center_x;
	wxTextCtrl *text_target_center_y;
	wxTextCtrl *text_beacon_center_x;
	wxTextCtrl *text_beacon_center_y;

	DECLARE_EVENT_TABLE()
};


#endif

