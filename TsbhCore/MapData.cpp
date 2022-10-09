#include "MapData.h"

MapData::MapData() {}
MapData::~MapData() {}
MapData::MapData(std::string URL1, std::string URL2, std::vector<zLayer*> layers)
	:base3dURL(URL1), base2dURL(URL2), layers(layers) {}