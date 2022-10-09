#pragma once
//#include <dtCore/system.h>
#include "rtsscore_global.h"
#include <string>

using std::string;
class RTSSCORE_EXPORT zMapEntity
{
public:
	zMapEntity();
	~zMapEntity();
	int selected;
	string image3dURL;
	string image2dURL;
	string elevationURL;
	void setSelected(int selected1);
};

