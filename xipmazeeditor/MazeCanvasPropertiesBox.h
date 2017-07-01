#ifndef _MAZECANVASPROPERTIESBOX_H_
#define _MAZECANVASPROPERTIESBOX_H_

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"


#include "Defines.h"
#include "MazeCanvas.h"


class MazeCanvasPropertiesBox : public wxDialog
{
public:
	MazeCanvasPropertiesBox(MyFrame * parent, MazeCanvas * maze_canvas);

	void showMazeCanvasPropertiesBox(double * width, double * height);

protected:
	void onClickOkButton(wxCommandEvent &event); 
	void onClickCancelButton(wxCommandEvent &event);

	MazeCanvas * maze_canvas;

	double * width;
	double * height;

	wxTextCtrl *text_width; 
	wxTextCtrl *text_height;


	DECLARE_EVENT_TABLE()
};


#endif

