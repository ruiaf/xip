#ifndef _MAZEFILEHANDLER_H_
#define _MAZEFILEHANDLER_H_

#include <list>

#include "wx/file.h"
#include "wx/filename.h"

#include "Defines.h"
#include "MazeXMLTag.h"



class MazeFileHandler
{
public:
	MazeFileHandler(MazeCanvas * maze_canvas, wxFileName * maze_file, wxFileName * grid_file);

	void setAllFiles(wxFileName * new_maze_file, wxFileName * new_grid_file);
	void setMazeFile(wxFileName * new_maze_file);
	void setGridFile(wxFileName * new_grid_file);

	int openAll(void);
	int openGridFile(void);
	int openMazeFile(void);
	

	int saveAll();
	int saveMazeFile(void);
	int saveGridFile(void);

protected:

	int openXMLFile(wxFileName * file_name);

	MazeXMLTag * temp_tag;

	MazeCanvas * maze_canvas;
	wxFileName * maze_file;
	wxFileName * grid_file;
};

#endif

