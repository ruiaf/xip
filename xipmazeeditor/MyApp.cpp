#include "MyApp.h"
#include "MyFrame.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	::wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame(wxString("XIP Maze Editor"),
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(TRUE);
    return TRUE;
}
