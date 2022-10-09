#pragma once
#include "rtsscore_global.h"
#include <osgDB/ReadFile>
#include <osgEarth/Map>
#include <osgEarth/MapNode>


class RTSSCORE_EXPORT QinxieModelLoader
{
public:
	QinxieModelLoader();
	~QinxieModelLoader();
	QinxieModelLoader(osg::ref_ptr<osgEarth::Map> map, osg::ref_ptr<osgEarth::MapNode> mapNode, std::string folder) :group(map)
	,rootFolder(folder),mapNode(mapNode){
		mStrSrs = "wgs84";
	};
	void loadModel(std::string name);
	void ReadMetaInfoFromLocal();
	void setLoadPosition(float lon, float lat, float alt);
protected:
	osg::ref_ptr<osgEarth::Map> group;
	osg::ref_ptr<osgEarth::MapNode> mapNode;
	std::string mStrSrs;
	std::string rootFolder;
	osg::Vec3d mVecPos;
	
	void str2Vec3d(std::vector<std::string> v, osg::Vec3d &p);

};