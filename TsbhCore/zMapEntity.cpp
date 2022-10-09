#include "zMapEntity.h"


zMapEntity::zMapEntity()
{
	selected = 0;
	image3dURL = "";
	image2dURL = "";
	elevationURL = "";
}


zMapEntity::~zMapEntity()
{
}


void zMapEntity::setSelected(int selected1)
{
	switch (selected1)
	{
	case 0:
		selected = 0;
		image3dURL = "http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/";
		image2dURL = "http://[abc].tile.openstreetmap.org/{z}/{x}/{y}.png";
		elevationURL = "http://data.marsgis.cn/terrain";
		break;
	case 1:
		selected = 1;
		image3dURL = "";
		image2dURL = "";
		elevationURL = "";
		break;
	
	}
}