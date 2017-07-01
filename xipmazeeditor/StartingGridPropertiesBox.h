#ifndef _STARTINGGRIDPROPERTIESBOX_H_
#define _STARTINGGRIDPROPERTIESBOX_H_

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"


#include "Defines.h"
#include "MazeCanvas.h"


class StartingGridPropertiesBox : public wxDialog
{
public:
	StartingGridPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas);

	void showStartingGridPropertiesBox(double * direction, int *pos_x, int *pos_y);

protected:
	void onClickOkButton(wxCommandEvent &event); 
	void onClickCancelButton(wxCommandEvent &event);

	void getIntValue(wxTextCtrl * text, int * value);

	MazeCanvas * maze_canvas;

	double *direction;
	int *pos_x; 
	int *pos_y;	

	wxTextCtrl *text_direction;
	wxTextCtrl *text_pos_x; 
	wxTextCtrl *text_pos_y;


	DECLARE_EVENT_TABLE()
};


#endif

