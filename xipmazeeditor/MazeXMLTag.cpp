#include "MazeXMLTag.h"

MazeXMLTag::MazeXMLTag()
{
	attributes.clear();
	close_tag = false;
	tag_complete = false;
	needs_close_tag = true;
	state = WAITING_FOR_OPEN_TAG;
	tag_name.Clear();
	temp_attribute_name.Clear();
	temp_attribute_value.Clear();
}

MazeXMLTag::~MazeXMLTag()
{
	emptyAttributes();
}


int MazeXMLTag::readString(char * buffer, int max_chars)
{
	int position;

	for (position=0; position >= 0 && position < max_chars && !isComplete(); position++)
	{
		switch(state)
		{
			case WAITING_FOR_OPEN_TAG: 
				if (buffer[position]=='<')
					state = READING_TAG_NAME;
					break;

			case READING_TAG_NAME: 		

				if (buffer[position]=='/')
					close_tag = true;

				else if (isalnum(buffer[position]))
					tag_name+=buffer[position];

				else if(tag_name.Length()>0)
				{
					position--;
					state = READING_ATTRIBUTE_NAME;
				}

				break;

			case READING_ATTRIBUTE_NAME: 
				if (buffer[position]=='/')
					needs_close_tag= false;

				else if (buffer[position] == '>')
					tag_complete=true;

				else if ( isalnum(buffer[position]) )
					temp_attribute_name+=buffer[position];

				else if (buffer[position]=='"')
					state = READING_ATTRIBUTE_VALUE;

				break;

			case READING_ATTRIBUTE_VALUE: 
				if (buffer[position]=='"')
				{

					attributes.push_back(new MazeXMLTagAttribute(temp_attribute_name, temp_attribute_value));
					temp_attribute_name.Clear();
					temp_attribute_value.Clear();
					state = READING_ATTRIBUTE_NAME;
				}

				else 
					temp_attribute_value+=buffer[position];

				break;
		}
	}

	if (isComplete())
		return position;
	else
		return -1;
}

bool MazeXMLTag::needsCloseTag(void)
{
	return needs_close_tag;
}

bool MazeXMLTag::isCloseTag(void)
{
	return close_tag;
}

bool MazeXMLTag::isComplete(void)
{
	return tag_complete;
}

bool MazeXMLTag::tagsMatch(MazeXMLTag &tag)
{
	return 
		((isCloseTag() && !tag.isCloseTag()) ||(!isCloseTag() && tag.isCloseTag())) &&
		getTagName() == tag.getTagName(); 
}

void MazeXMLTag::executeTag(MazeCanvas * maze_canvas)
{

	static Wall * temp_wall = NULL;

	wxString temps;

	if( tag_name.IsSameAs("Lab",false) )
	{
		if (isCloseTag())
			return;

		double height=50, width=50;
		

		temps = "Width";
		getAttributeValue(temps, width);
		temps = "Height";
		getAttributeValue(temps, height);
		
		maze_canvas->setCanvasSize(width, height);
		
	}
	else if (tag_name.IsSameAs("Beacon",false))
	{
		double x=0,y=0,height=DefaultConfs::getDEFAULT_BEACON_HEIGHT();

		temps="Height";
		getAttributeValue(temps,height);
		temps="X";
		getAttributeValue(temps,x);
		temps="Y";
		getAttributeValue(temps,y);


		Beacon * beacon = maze_canvas->getBeacon();

		if (beacon == NULL)
		{
			beacon = new Beacon((int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()),maze_canvas->getZoomFactor(),(float)height);
			maze_canvas->addNewObject(beacon);
		}
		else
		{
			beacon->setBeaconPosition((int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()));
			beacon->setHeight((float)height);
		}
	}
	else if (tag_name.IsSameAs("Target",false))
	{
		double x=0,y=0,radius=DefaultConfs::getDEFAULT_BEACON_RADIUS();

		temps = "Radius";
		getAttributeValue(temps,radius);
		temps = "X";
		getAttributeValue(temps,x);
		temps = "Y";
		getAttributeValue(temps,y);


		Beacon * beacon = maze_canvas->getBeacon();

		if (beacon == NULL)
		{
			beacon = new Beacon((int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()),(float)(maze_canvas->getZoomFactor()*radius),DefaultConfs::getDEFAULT_BEACON_HEIGHT());
			maze_canvas->addNewObject(beacon);
		}
		else
		{
			beacon->setTargetPosition((int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()));
			beacon->setRadius((float)(maze_canvas->getZoomFactor()*radius));
		}
	}
	else if (tag_name.IsSameAs("Wall",false))
	{
		if (isCloseTag())
		{
			if (temp_wall!=NULL)
			{
				temp_wall->close();
				maze_canvas->addNewObject(temp_wall);
				temp_wall = NULL;				
			}
			return;
		}

		double height;
		temps = "Height";
		getAttributeValue(temps, height);
		temp_wall = new Wall(height);
	}
	else if (tag_name.IsSameAs("Corner",false))
	{
		
		if (temp_wall != NULL)
		{
			double x=0, y=0;
			temps = "X";
			getAttributeValue(temps,x);
			temps = "Y";
			getAttributeValue(temps,y);
			temp_wall->addPoint((int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()));
		}
	}
	else if (tag_name.IsSameAs("Grid",false))
	{	
		//ignore this tag since it has no attributes
	}
	else if (tag_name.IsSameAs("Position",false))
	{		
		double x=0,y=0,dir=0;
		
		temps = "X";
		getAttributeValue(temps,x);
		temps = "Y";
		getAttributeValue(temps,y);
		temps = "Dir";
		getAttributeValue(temps, dir);

		maze_canvas->addNewObject(new StartingGrid(maze_canvas->getFirstStartingGridUnusedNumber(), (int)(x*maze_canvas->getZoomFactor()),(int)(y*maze_canvas->getZoomFactor()), dir));	
	}	
}


wxString * MazeXMLTag::getAttribute(wxString & attrib_name)
{	
	for(std::list<MazeXMLTagAttribute *>::const_iterator iter = attributes.begin(); iter != attributes.end() ; iter++)
		if ( ((MazeXMLTagAttribute *)*iter)->getName().IsSameAs(attrib_name,false) )
			return &(((MazeXMLTagAttribute *)*iter)->getValue());

	return NULL;
		
}


bool MazeXMLTag::getAttributeValue(wxString & attrib_name, double &value)
{
	wxString * temp = getAttribute(attrib_name);

	if (temp==NULL)
		return false;

	return temp->ToDouble(&value);	
}


void MazeXMLTag::emptyAttributes(void)
{
	for(std::list<MazeXMLTagAttribute *>::const_iterator iter = attributes.begin(); iter != attributes.end() ; iter++)
		delete *iter;
	attributes.empty();
}

