#ifndef _MAZEXMLTAGATTRIBUTE_H_
#define _MAZEXMLTAGATTRIBUTE_H_

#include "wx/string.h"

#include "Defines.h"


class MazeXMLTagAttribute
{
public:
	MazeXMLTagAttribute(){}
	MazeXMLTagAttribute(wxString name, wxString value):name(name),value(value){}
	~MazeXMLTagAttribute(){}
	wxString & getName(void){return name;}
	wxString & getValue(void){return value;}

protected:

	wxString name;
	wxString value;

};


#endif

