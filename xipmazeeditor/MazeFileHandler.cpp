#include "MazeFileHandler.h"

MazeFileHandler::MazeFileHandler(
	MazeCanvas * maze_canvas, 
	wxFileName * maze_file, 
	wxFileName * grid_file)
	:
	maze_canvas(maze_canvas),
	maze_file(maze_file),
	grid_file(grid_file)
{
		temp_tag = NULL;
}

void MazeFileHandler::setAllFiles(wxFileName * new_maze_file, wxFileName * new_grid_file)
{
	setMazeFile(new_maze_file);
	setGridFile(new_grid_file);
}

void MazeFileHandler::setMazeFile(wxFileName * new_maze_file)
{
	maze_file = new_maze_file;
}

void MazeFileHandler::setGridFile(wxFileName * new_grid_file)
{
	grid_file = new_grid_file;
}

int MazeFileHandler::openAll(void)
{
	openMazeFile();
	openGridFile();
	maze_canvas->doMirrorOnY();
	maze_canvas->forceRefresh();
	return 0;
}

int MazeFileHandler::openGridFile(void)
{
	return openXMLFile(grid_file);
}

int MazeFileHandler::openMazeFile(void)
{
	return openXMLFile(maze_file);;
}

int MazeFileHandler::openXMLFile(wxFileName * file_name)
{
	if (!wxFile::Exists(file_name->GetFullPath()))
		return -1;

	wxFile temp_file(file_name->GetFullPath());
	
	if (!temp_file.IsOpened())
		return -1;

	char buffer[READ_BUFFER_SIZE];
	char * aux = NULL;
	int n_chars_read=-1, total_chars_read=0;

	temp_tag = new MazeXMLTag();


	while(!(temp_file.Eof() && n_chars_read==-1))
	{
		if (n_chars_read==-1)
		{
			total_chars_read = temp_file.Read(buffer, READ_BUFFER_SIZE);
			aux = buffer;
		}
		else
		{
			aux += n_chars_read;
		}
		
		n_chars_read = temp_tag->readString(aux, total_chars_read-(aux-buffer));
		
		if (temp_tag->isComplete())
		{
			temp_tag->executeTag(maze_canvas);
			delete temp_tag;
			temp_tag = new MazeXMLTag();
		}
	}

	delete temp_tag;

	temp_file.Close();

	maze_canvas->forceRefresh();

	return 0;
}



int MazeFileHandler::saveAll()
{
	maze_canvas->doMirrorOnY();
	saveMazeFile();
	saveGridFile();
	maze_canvas->doMirrorOnY();
	maze_canvas->forceRefresh();
	return 0;
}

int MazeFileHandler::saveMazeFile(void)
{
	wxFile file(maze_file->GetFullPath(), wxFile::write);

	if ( !file.IsOpened() )
		return -1;
	
	int ret_val  =  maze_canvas->toXmlMazeFile(file);
	
	file.Close();

	return ret_val;
}

int MazeFileHandler::saveGridFile(void)
{
	wxFile file(grid_file->GetFullPath(), wxFile::write);

	if ( !file.IsOpened() )
		return -1;
	
	int ret_val  =  maze_canvas->toXmlGridFile(file);
	
	file.Close();

	return ret_val;
}
