#include "MazeCanvas.h"

#include <wx/dcclient.h>

BEGIN_EVENT_TABLE(MazeCanvas, wxWindow)
    EVT_LEFT_DOWN (				MazeCanvas::mouseLeftButtonDown		)
	EVT_LEFT_UP(				MazeCanvas::mouseLeftButtonUp		)
	EVT_LEFT_DCLICK(			MazeCanvas::mouseLeftDoubleClick	)
    EVT_RIGHT_DOWN(				MazeCanvas::mouseRightButtonDown	)
	EVT_LEAVE_WINDOW(			MazeCanvas::mouseLeftWindow			)
    EVT_MOTION(					MazeCanvas::mouseMoved				)
    EVT_PAINT(					MazeCanvas::onPaint					)
    EVT_ERASE_BACKGROUND(		MazeCanvas::onErase					)
	EVT_KEY_DOWN(				MazeCanvas::keyDown					)
	//EVT_SIZE(					MazeCanvas::onSize					)
	EVT_SCROLLWIN_TOP(			MazeCanvas::scrollWinTop			)
	EVT_SCROLLWIN_BOTTOM(		MazeCanvas::scrollWinBottom			)
	EVT_SCROLLWIN_LINEUP(		MazeCanvas::scrollWinLineUp			)
	EVT_SCROLLWIN_LINEDOWN(		MazeCanvas::scrollWinLineDown		)
	EVT_SCROLLWIN_PAGEUP(		MazeCanvas::scrollWinPageUp			)
	EVT_SCROLLWIN_PAGEDOWN(		MazeCanvas::scrollWinPageDown		)
	EVT_SCROLLWIN_THUMBTRACK(	MazeCanvas::scrollWinThumbTrack		)
	EVT_SCROLLWIN_THUMBRELEASE(	MazeCanvas::scrollWinThumbRelease	)
END_EVENT_TABLE()


const wxBrush MazeCanvas::IMAGE_BACKGROUND_BRUSH(wxColour(0,0,0),wxSOLID);
const wxPen MazeCanvas::IMAGE_BACKGROUND_PEN(wxColour(0,0,0),1,wxSOLID);

const wxBrush MazeCanvas::BACKGROUND_BRUSH(wxColour(220,220,220),wxSOLID);
const wxPen MazeCanvas::BACKGROUND_PEN(wxColour(220,220,220),1,wxSOLID);

const wxPen MazeCanvas::GRID_PEN(wxColour(220,220,220),1,wxSOLID);



MazeCanvas::MazeCanvas(MyFrame * my_frame, wxWindowID id, double new_canvas_width, double new_canvas_height)
:
top_left_canvas_corner_x(0),
top_left_canvas_corner_y(0),
canvas_current_width(new_canvas_width),
canvas_current_height(new_canvas_height),
zoom_factor(DefaultConfs::getDEFAULT_ZOOM_FACTOR()),
my_frame(my_frame),
background_buffer(NULL),
background_buffer_bmp(NULL),
grid_pixel_increment((int)DEFAULT_GRID_PIXEL_INCREMENT),
mixing_buffer(NULL),
mixing_buffer_bmp(NULL),
window_current_width ( (int)(zoom_factor*new_canvas_width ) ),
window_current_height( (int)(zoom_factor*new_canvas_height) ),
wxWindow(my_frame,id, wxPoint(0,0),wxSize(window_current_width,window_current_height))
{
	active_tool=-1;

	grid_enabled = true;

	tools = new MazeTool * [TOOL_QUANTITY];
	active_tool = -1;
	tools[BEACON_TOOL] = new BeaconTool(this);
	tools[STARTING_GRID_TOOL] = new StartingGridTool(this);
	tools[HIGH_WALL_TOOL] = new WallTool(this,(float)DefaultConfs::getDEFAULT_HIGH_WALL_HEIGHT());
	tools[LOW_WALL_TOOL] = new WallTool(this,(float)DefaultConfs::getDEFAULT_LOW_WALL_HEIGHT());
	tools[ARROW_TOOL] = new ArrowTool(this);


	setCanvasSize(canvas_current_width, canvas_current_height);

	initializeBitmapsMemoryDCs(window_current_width + 8, window_current_height + 8);

	SetScrollbar(wxVERTICAL, 0, 16,40);

	unselected_objects.clear();

}

MazeCanvas::~MazeCanvas(void)
{
	clearAllCanvas();
	for( int x = 0 ; x < TOOL_QUANTITY ; x++)
		delete tools[x];
	delete [] tools;
}

void MazeCanvas::mouseRightButtonDown(wxMouseEvent & event)
{
	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;
	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->mouseRightButtonDown(event);
	}
}

void MazeCanvas::mouseLeftButtonDown(wxMouseEvent & event)
{
	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;
    if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->mouseLeftButtonDown(event);
	}
}

void MazeCanvas::mouseLeftButtonUp(wxMouseEvent & event)
{
	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;
	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->mouseLeftButtonUp(event);
	}
}

void MazeCanvas::mouseMoved(wxMouseEvent & event)
{
	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;

	my_frame->setCoordinates(((float)event.m_x)/(float)zoom_factor,
							 ((float)window_current_height-event.m_y)/(float)zoom_factor);


	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->mouseMoved(event);
	}
}



void MazeCanvas::keyDown(wxKeyEvent & event)
{
	if (event.GetKeyCode() == 'A')
	{
		debug4d((int)canvas_current_width*this->getZoomFactor(), (int)canvas_current_height*this->getZoomFactor(), (int)top_left_canvas_corner_x, (int)top_left_canvas_corner_y);
	}

	if (event.GetKeyCode() == 'B')
	{
		adjustCanvas();
	}

	if (event.GetKeyCode() == 'C')
	{
		debug4d(top_left_canvas_corner_x, top_left_canvas_corner_y,0,0);
	}
	if (event.GetKeyCode() == 'D')
	{
		setTopLeftOffsets(0,0);
	}

	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;

	my_frame->setCoordinates(((float)event.m_x)/(float)zoom_factor,
							 ((float)event.m_y)/(float)zoom_factor);

	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->keyDown(event);

	}
}

void MazeCanvas::onPaint(wxPaintEvent & event)
{
	CenterOnParent();
    wxPaintDC dc(this);

	mixing_buffer->Blit(0, 0, window_current_width, window_current_height, background_buffer, 0, 0);
	paintForeground(mixing_buffer);

    dc.Blit(0, 0, window_current_width, window_current_height, mixing_buffer, 0, 0);
}

void MazeCanvas::onErase(wxEraseEvent  & event)
{
}

void MazeCanvas::paintBackground(wxDC * dc)
{
	background_buffer->SetBrush(BACKGROUND_BRUSH);
	background_buffer->SetPen(BACKGROUND_PEN);
	dc->DrawRectangle(0,0,window_current_width,window_current_height);

	background_buffer->SetBrush(IMAGE_BACKGROUND_BRUSH);
	background_buffer->SetPen(IMAGE_BACKGROUND_PEN);
	dc->DrawRectangle(0, 0,window_current_width,window_current_height);
	paintGrid(dc, -top_left_canvas_corner_x%grid_pixel_increment, -top_left_canvas_corner_y%grid_pixel_increment, window_current_width,window_current_height);

	for (std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
	{
		(*iter)->paint(dc,-top_left_canvas_corner_x, -top_left_canvas_corner_y);
	}
}

void MazeCanvas::paintForeground(wxDC * dc)
{
	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		tools[active_tool]->paint(dc,-top_left_canvas_corner_x, -top_left_canvas_corner_y);
	}
}


void MazeCanvas::setActiveTool(int tool)
{
	if ( active_tool != tool && active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		tools[active_tool]->resetTool();
	}

	active_tool = tool;

	if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		tools[active_tool]->resetTool();
	}
	my_frame->setActiveToolBarToolButton(active_tool);
	Refresh();
}


void MazeCanvas::addNewObject(MazeObject * mo)
{
	unselected_objects.push_back(mo);
	paintBackground(background_buffer);
	verifyConstraints();
}

MazeObject * MazeCanvas::getSelectedObject(int x, int y)
{
	if (unselected_objects.empty())
		return NULL;

	std::list<MazeObject *>::const_iterator iter = unselected_objects.end();

	do
	{
		iter--;
		if ( (*iter)->isHited(x,y) )
		{
			MazeObject * temp = *iter;
			unselected_objects.remove(*iter);
			paintBackground(background_buffer);
			return temp;
		}
	}while(iter != unselected_objects.begin());

	return NULL;
}


void MazeCanvas::initializeBitmapsMemoryDCs(int width, int height)
{
	if (background_buffer == NULL)
		background_buffer = new wxMemoryDC();

	if (background_buffer_bmp != NULL)
		delete background_buffer_bmp;

    background_buffer_bmp = new wxBitmap(width,height);
	background_buffer_bmp->SetWidth(width);
	background_buffer_bmp->SetHeight(height);
    background_buffer->SelectObject(*background_buffer_bmp);

	if (mixing_buffer ==  NULL)
		mixing_buffer = new wxMemoryDC();

	if (mixing_buffer_bmp != NULL)
		delete mixing_buffer_bmp;

    mixing_buffer_bmp = new wxBitmap(width,height);
	mixing_buffer_bmp->SetWidth(width);
	mixing_buffer_bmp->SetHeight(height);
    mixing_buffer->SelectObject(*mixing_buffer_bmp);
}



void MazeCanvas::mouseLeftWindow(wxMouseEvent & event)
{
	if (false && active_tool != ARROW_TOOL)
		setActiveTool(NO_TOOL);
}


bool MazeCanvas::isInsideCanvas(int x, int y)
{
	return	x >= top_left_canvas_corner_x &&
			x <= top_left_canvas_corner_x+window_current_width &&
			y >= top_left_canvas_corner_y &&
			y <= top_left_canvas_corner_y + window_current_height;
}


void MazeCanvas::paintGrid(wxDC * dc, int start_x, int start_y, int width, int height)
{
	if (!grid_enabled)
		return;
	dc->SetPen(GRID_PEN);
	for (int x = 0; x < width; x+=grid_pixel_increment)
		for (int y = 0; y < height; y+=grid_pixel_increment)
			dc->DrawPoint(x+start_x,y+start_y);
}

void MazeCanvas::snapToGrid(int &x, int &y)
{
	if (!grid_enabled)
		return;
	x = ((int)(0.5+(float)x/grid_pixel_increment))*grid_pixel_increment;
	y = ((int)(0.5+(float)y/grid_pixel_increment))*grid_pixel_increment;
}

void MazeCanvas::clearAllCanvas(void)
{
	setActiveTool(NO_TOOL);
	if ( !unselected_objects.empty() )
	{
		for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
			delete (*iter);
		unselected_objects.clear();
		paintBackground(background_buffer);
		Refresh();
	}
	verifyConstraints();
}


void MazeCanvas::setCanvasSize(double new_canvas_width, double new_canvas_height)
{
	canvas_current_width = new_canvas_width;
	canvas_current_height = new_canvas_height;

	window_current_width = (int)(canvas_current_width * zoom_factor);
	window_current_height = (int)(canvas_current_height * zoom_factor);

	SetSize(window_current_width, window_current_height);

	initializeBitmapsMemoryDCs(window_current_width + 8, window_current_height + 8);

	setTopLeftOffsets(0,0);

	adjustCanvas();
}

Beacon * MazeCanvas::getBeacon(void)
{
	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
			if ((*iter)->getType() == TYPE_BEACON)
				return (Beacon *) *iter;

	return NULL;
}

void MazeCanvas::forceRefresh(void)
{
	paintBackground(background_buffer);
	Refresh();
}

void MazeCanvas::setZoomFactor(double new_zoom_factor)
{
	zoom_factor = new_zoom_factor;
}

double MazeCanvas::getZoomFactor(void)
{
	return zoom_factor;
}


void MazeCanvas::setTopLeftOffsets(int x, int y)
{
	top_left_canvas_corner_x = x;
	top_left_canvas_corner_y = y;
}


int MazeCanvas::toXmlMazeFile(wxFile & file_to_write_in)
{
	int written_bytes=0;
	char buffer[1024];

	setActiveTool(NO_TOOL);

	sprintf(buffer, "<Lab Name=\"Xip Maze Editor\" Width=\"%lf\" Height=\"%lf\">\n\n", canvas_current_width, canvas_current_height);
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));

	Beacon * b = getBeacon();

	if (b != NULL)
		written_bytes += b->toXmlFile(file_to_write_in, this);

	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
			if ((*iter)->getType() == TYPE_HIGH_WALL || (*iter)->getType() ==TYPE_LOW_WALL)
				written_bytes += (*iter)->toXmlFile(file_to_write_in, this);


	sprintf(buffer, "</Lab>\n");
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));

	return written_bytes;
}

int MazeCanvas::toXmlGridFile(wxFile & file_to_write_in)
{
	int written_bytes=0;
	char buffer[1024];

	setActiveTool(NO_TOOL);

	sprintf(buffer, "<Grid>\n\n");
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));

	for(int pos = 1 ; pos < 4 ; pos++)
		for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
				if ((*iter)->getType() == TYPE_STARTING_GRID && ((StartingGrid *)(*iter))->getNumber() == pos)
					written_bytes += (*iter)->toXmlFile(file_to_write_in, this);

	sprintf(buffer, "</Grid>\n");
	written_bytes += file_to_write_in.Write(buffer, strlen(buffer));


	return written_bytes;
}

void MazeCanvas::verifyConstraints(void)
{
	int nwalls=0, nstartgrids=0, nbeacons=0;
	getNumberMazeObjects(nbeacons, nstartgrids, nwalls);


	if ( nbeacons >= 1 )
	{
		enableTool(BEACON_TOOL, false);
		if (active_tool == BEACON_TOOL)
			setActiveTool(ARROW_TOOL);
	}
	else
		enableTool(BEACON_TOOL, true);

	if ( nstartgrids >= 3 )
	{
		enableTool(STARTING_GRID_TOOL, false);
		if (active_tool == STARTING_GRID_TOOL)
			setActiveTool(ARROW_TOOL);
	}
	else
		enableTool(STARTING_GRID_TOOL, true);


}

void MazeCanvas::getNumberMazeObjects(int &nbeacons, int &nstartgrids, int &nwalls)
{
	nbeacons = nstartgrids = nwalls = 0;

	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
	{
			if ((*iter)->getType() == TYPE_STARTING_GRID)
				nstartgrids++;
			else if ((*iter)->getType() == TYPE_BEACON)
				nbeacons++;
			else if((*iter)->getType() == TYPE_HIGH_WALL || (*iter)->getType() ==TYPE_LOW_WALL)
				nwalls++;
	}

}


void MazeCanvas::enableTool(int tool, bool enable)
{
	my_frame->enableToolBarTool(tool, enable);
}


void MazeCanvas::adjustCanvas(void)
{
	int width, height, cw, ch;

	width = window_current_width;
	height = window_current_height;

	my_frame->GetClientSize(&cw, &ch);


	if (width > cw)
	{
		width = cw;
		SetScrollbar(wxHORIZONTAL, 0, width, window_current_width);
	}
	else
		SetScrollbar(wxHORIZONTAL, 0, 0, 0);


	if (height > ch)
	{
		height = ch ;
		SetScrollbar(wxVERTICAL, 0, height,window_current_height);
	}
	else
		SetScrollbar(wxVERTICAL, 0, 0, 0);

	SetSize(width, height);

	setTopLeftOffsets(0,0);

	my_frame->Refresh();
}

void MazeCanvas::scrollWinThumbTrack(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), event.GetPosition());
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}

void MazeCanvas::scrollWinThumbRelease(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), event.GetPosition());
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}


void MazeCanvas::scrollWinTop(wxScrollWinEvent &event)
{
	debug1s("este");
}

void MazeCanvas::scrollWinBottom(wxScrollWinEvent &event)
{
	debug1s("este2");
}

void MazeCanvas::scrollWinLineUp(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), GetScrollPos(event.GetOrientation())-1);
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}

void MazeCanvas::scrollWinLineDown(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), GetScrollPos(event.GetOrientation())+1);
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}


void MazeCanvas::scrollWinPageUp(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), GetScrollPos(event.GetOrientation())-200);
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}

void MazeCanvas::scrollWinPageDown(wxScrollWinEvent &event)
{
	SetScrollPos(event.GetOrientation(), GetScrollPos(event.GetOrientation())+200);
	setTopLeftOffsets(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	forceRefresh();
}

void MazeCanvas::onSize(wxSizeEvent & event)
{
	debug1s("MazeCanvas::onSize");
	adjustCanvas();
}

int MazeCanvas::getWindowCurrentWidth(void)
{
	return window_current_width;
}

int MazeCanvas::getWindowCurrentHeight(void)
{
	return window_current_height;
}


void MazeCanvas::mouseLeftDoubleClick(wxMouseEvent & event)
{
	event.m_x+=top_left_canvas_corner_x;
	event.m_y+=top_left_canvas_corner_y;
	if (!isInsideCanvas(event.m_x,event.m_y))
		return;
    if ( active_tool >= 0 && active_tool < TOOL_QUANTITY)
	{
		snapToGrid(event.m_x, event.m_y);
		tools[active_tool]->mouseLeftDoubleClick(event);
	}
}


void MazeCanvas::doMirrorOnY(void)
{
	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
			(*iter)->doMirrorOnY(window_current_height);
}


int MazeCanvas::getFirstStartingGridUnusedNumber()
{
	int x = 1;

	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
	{
			if ((*iter)->getType() == TYPE_STARTING_GRID)
				if ( ((StartingGrid *)*iter)->getNumber() == x)
				{
					x++;
					iter = unselected_objects.begin();
					iter--;
				}
	}
	return x;

}


void MazeCanvas::setGridEnabled(bool enable)
{
	grid_enabled = enable;
	forceRefresh();
}



void MazeCanvas::showPropertiesBox()
{
	if(MyFrame::maze_canvas_properties_box == NULL)
		return;

	MyFrame::maze_canvas_properties_box->showMazeCanvasPropertiesBox(&canvas_current_width, &canvas_current_height);

	setCanvasSize(canvas_current_width, canvas_current_height);

	forceRefresh();
}


int MazeCanvas::exportWorldState(wxFile & file_to_write_in, double resolution)
{
	int written_bytes = 0;

	setActiveTool(NO_TOOL);

	Beacon * beacon = getBeacon();
	StartingGrid * starting_grid = NULL;

	if(beacon == NULL)
		return -1;

	int x, y;
	double dir;

	wxString temp;


	doMirrorOnY();

	for(int pos = 1 ; pos < 4 ; pos++)
		for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
				if ((*iter)->getType() == TYPE_STARTING_GRID && ((StartingGrid *)(*iter))->getNumber() == pos)
				{
					starting_grid = (StartingGrid *)(*iter);
					starting_grid->getPosition(x,y);
					dir = starting_grid->getDirection();
					temp.Empty();
					temp.Printf("grid %lf %lf %lf\n", dir, ((double)x) / getZoomFactor(), ((double)y)/getZoomFactor());
					written_bytes += file_to_write_in.Write(temp);
				}


	temp.Empty();
	beacon->getBeaconPosition(x,y);
	temp.Printf("beacon %lf %lf\n", ((double)x) / getZoomFactor(), ((double)y)/getZoomFactor());
	written_bytes += file_to_write_in.Write(temp);

	doMirrorOnY();

	wxBitmap bitmap(window_current_width, window_current_height);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(bitmap);


	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
		if ((*iter)->getType() == TYPE_HIGH_WALL || (*iter)->getType() == TYPE_LOW_WALL)
			(*iter)->paint(&temp_dc, top_left_canvas_corner_x, top_left_canvas_corner_y);

	wxImage a = bitmap.ConvertToImage();
	wxImage b = a.Scale((int) (canvas_current_width * resolution), (int) (canvas_current_height * resolution));
	wxImage c = b.ConvertToMono(0,0,0);

	unsigned char * buffer = c.GetData();

	temp.Empty();
	int size = (int)(canvas_current_width * resolution * canvas_current_height * resolution);
	int line_size = (int)(canvas_current_width * resolution);


	temp.Empty();
	for (int n = 0 ; n < size ; n++)
	{
		if ( buffer[n*3] > 0)
			temp.Append("0");
		else
			temp.Append("1");

		if ((n+1) % line_size == 0)
		{
			temp.Append("\n");
			written_bytes += file_to_write_in.Write(temp);
			temp.Empty();
		}
	}

	return written_bytes;
}


StartingGrid * MazeCanvas::getStartingGrid(int grid_position)
{
	for(std::list<MazeObject *>::const_iterator iter = unselected_objects.begin(); iter != unselected_objects.end() ; iter++)
		if ((*iter)->getType() == TYPE_STARTING_GRID && ((StartingGrid *)(*iter))->getNumber() == grid_position)
			return (StartingGrid *)(*iter);

	return NULL;
}

