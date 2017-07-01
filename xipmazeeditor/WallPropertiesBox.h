#ifndef _WALLPROPERTIESBOX_H_
#define _WALLPROPERTIESBOX_H_

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"


#include "Defines.h"
#include "MazeCanvas.h"


class WallPropertiesBox : public wxDialog
{
public:
	WallPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas);

	void showWallPropertiesBox(float *wall_height);

protected:
	void onClickOkButton(wxCommandEvent &event); 
	void onClickCancelButton(wxCommandEvent &event);

	void getIntValue(wxTextCtrl * text, int * value);

	MazeCanvas * maze_canvas;

	float *wall_height;

	wxTextCtrl *text_wall_height;

	DECLARE_EVENT_TABLE()
};


#endif

