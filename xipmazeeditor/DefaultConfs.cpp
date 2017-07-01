#include "DefaultConfs.h"

double DefaultConfs::DEFAULT_CANVAS_WIDTH = 28.0;
double DefaultConfs::DEFAULT_CANVAS_HEIGHT = 14.0;
double DefaultConfs::MAXIMUM_LOW_WALL_HEIGHT = 1;
double DefaultConfs::DEFAULT_LOW_WALL_HEIGHT = 1;
double DefaultConfs::DEFAULT_HIGH_WALL_HEIGHT = 2.5;
double DefaultConfs::DEFAULT_BEACON_RADIUS = 30;
double DefaultConfs::DEFAULT_BEACON_HEIGHT = 30;
double DefaultConfs::STARTING_GRID_PAINTED_SQUARE_WIDTH = 20;
double DefaultConfs::STARTING_GRID_PAINTED_SQUARE_HEIGHT = DefaultConfs::STARTING_GRID_PAINTED_SQUARE_WIDTH;
double DefaultConfs::DEFAULT_ZOOM_FACTOR = 24.0;
double DefaultConfs::EXPORT_RESOLUTION = 10.0;

void DefaultConfs::loadDefaultConfsFromFile()
{
	if (!wxFile::Exists(CONF_FILE_PATH))
		return;

	wxFile file(CONF_FILE_PATH);

	if (!file.IsOpened())
		return;

	char buffer[READ_BUFFER_SIZE];
	wxString line;
	int bytes_read=0;

	line.Empty();

	do
	{
		bytes_read = file.Read(buffer, READ_BUFFER_SIZE-2);

		if (bytes_read == -1 )
			bytes_read = 0;
		buffer[bytes_read] = 0;
		line.append(buffer);

		while ( -1 != (bytes_read = line.Find('\n')) )
		{	
			wxString temps = line.Mid(0,bytes_read+1);
			interpretLine(temps);
			line = line.Mid(bytes_read+1);
		}
	}while( !file.Eof());

	if ( !line.IsEmpty())
		interpretLine(line);

	file.Close();
	
}

void DefaultConfs::interpretLine(wxString & line)
{
	double value;
	char var_name[READ_BUFFER_SIZE];

	sscanf((const char *)line, "%s = %lf", var_name, &value);

	if ( 0==strcmp(var_name, "DEFAULT_CANVAS_WIDTH") )
	{
		DEFAULT_CANVAS_WIDTH = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_CANVAS_HEIGHT") )
	{		
		DEFAULT_CANVAS_HEIGHT = value;
	}
	else if ( 0==strcmp(var_name, "MAXIMUM_LOW_WALL_HEIGHT") )
	{
		MAXIMUM_LOW_WALL_HEIGHT = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_LOW_WALL_HEIGHT") )
	{
		DEFAULT_LOW_WALL_HEIGHT = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_HIGH_WALL_HEIGHT") )
	{
		DEFAULT_HIGH_WALL_HEIGHT = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_BEACON_RADIUS") )
	{
		DEFAULT_BEACON_RADIUS = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_BEACON_HEIGHT") )
	{
		DEFAULT_BEACON_HEIGHT = value;
	}
	else if ( 0==strcmp(var_name, "STARTING_GRID_PAINTED_SQUARE_WIDTH") )
	{
		STARTING_GRID_PAINTED_SQUARE_WIDTH = value;
	}
	else if ( 0==strcmp(var_name, "DEFAULT_ZOOM_FACTOR") )
	{
		DEFAULT_ZOOM_FACTOR = value;
	}
	else if ( 0==strcmp(var_name, "EXPORT_RESOLUTION") )
	{
		EXPORT_RESOLUTION = value;
	}
}

double DefaultConfs::getDEFAULT_CANVAS_WIDTH()
{
	return DEFAULT_CANVAS_WIDTH;
}

double DefaultConfs::getDEFAULT_CANVAS_HEIGHT()
{
	return DEFAULT_CANVAS_HEIGHT;
}

double DefaultConfs::getMAXIMUM_LOW_WALL_HEIGHT()
{
	return MAXIMUM_LOW_WALL_HEIGHT;
}

double DefaultConfs::getDEFAULT_LOW_WALL_HEIGHT()
{
	return DEFAULT_LOW_WALL_HEIGHT;
}


double DefaultConfs::getDEFAULT_HIGH_WALL_HEIGHT()
{
	return DEFAULT_HIGH_WALL_HEIGHT;
}


double DefaultConfs::getDEFAULT_BEACON_RADIUS()
{
	return DEFAULT_BEACON_RADIUS;
}

double DefaultConfs::getDEFAULT_BEACON_HEIGHT()
{
	return DEFAULT_BEACON_HEIGHT;
}

double DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_WIDTH()
{
	return STARTING_GRID_PAINTED_SQUARE_WIDTH;
}

double DefaultConfs::getSTARTING_GRID_PAINTED_SQUARE_HEIGHT()
{
	return STARTING_GRID_PAINTED_SQUARE_HEIGHT;
}

double DefaultConfs::getDEFAULT_ZOOM_FACTOR()
{
	return DEFAULT_ZOOM_FACTOR;
}


double DefaultConfs::getEXPORT_RESOLUTION()
{
	return EXPORT_RESOLUTION;
}
