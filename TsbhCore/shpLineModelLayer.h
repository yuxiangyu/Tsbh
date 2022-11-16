#pragma once
#include "rtsscore_global.h"
#include "zeVectorLayer.h"
#include <osgEarth/MapNode>
#include <osgEarth/Feature>

class RTSSCORE_EXPORT shpLineModelLayer : public zeVectorLayer
{
public:
	shpLineModelLayer();
	shpLineModelLayer(std::string name, std::string filePath);
	~shpLineModelLayer();
	void addToScene(osgEarth::MapNode* root);
	void remove();


public:
	osgEarth::Color lineColor = osgEarth::Color("#707070ff");
	double lineWidth = 1.0f;
	//ÇÐÆ¬·¶Î§
	double minrange = 0.1;

private:
	osgEarth::MapNode* _root;
	osgEarth::FeatureList listFeature;
	osg::ref_ptr<osg::LOD> lod;
};

