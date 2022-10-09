#pragma once
#include "rtsscore_global.h"
//#include <boost/serialization/vector.hpp>
//#include <boost/archive/xml_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>
#include "zlayer.h"

class RTSSCORE_EXPORT MapData
{
public:
	MapData() ;
	~MapData() ;
	MapData(std::string URL1, std::string URL2, std::vector<zLayer*> layers);
	//��ά��ͼ
	std::string base3dURL;
	//��ά��ͼ
	std::string base2dURL;

	std::vector<zLayer*> layers;


	
};