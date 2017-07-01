#ifndef _MAZEXMLTAG_H_
#define _MAZEXMLTAG_H_

#include <list>
#include <ctype.h>

#include "wx/string.h"

#include "Defines.h"
#include "MazeXMLTagAttribute.h"
#include "MazeCanvas.h"


enum xml_reading_states
{
	WAITING_FOR_OPEN_TAG,
	READING_TAG_NAME,
	READING_ATTRIBUTE_NAME,
	READING_ATTRIBUTE_VALUE
};

class MazeXMLTag
{
public:

	MazeXMLTag();
	~MazeXMLTag();

	wxString & getTagName(){return tag_name;}

	/* returns -1 while tag still not complete, otherwise returns number of char's read */
	int readString(char * buffer, int max_chars);

	void executeTag(MazeCanvas * maze_canvas);

	wxString * getAttribute(wxString & attrib_name);
	bool getAttributeValue(wxString & attrib_name, double &value);

	bool needsCloseTag(void);
	bool isCloseTag(void);
	bool isComplete(void);
	bool tagsMatch(MazeXMLTag &tag);

	
protected:

	void emptyAttributes(void);

	xml_reading_states state;

	bool tag_complete;
	bool needs_close_tag;
	bool close_tag;

	wxString tag_name;

	wxString temp_attribute_name;
	wxString temp_attribute_value;
	
	std::list<MazeXMLTagAttribute *> attributes;
};

#endif
