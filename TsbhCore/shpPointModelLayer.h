#pragma once

#pragma once
#include "rtsscore_global.h"
#include "zlayer.h"
#include "zeVectorLayer.h"
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osgEarth/TextSymbol>
#include <osgEarth/IconSymbol>
#include <osgEarth/PlaceNode>

class RTSSCORE_EXPORT shpPointModelLayer : public zeVectorLayer
{
public:
	shpPointModelLayer();
	shpPointModelLayer(std::string name, std::string filePath);
	~shpPointModelLayer(void);
	void addToScene(osgEarth::MapNode* root);
	void remove();
	//颜色
	osg::Vec4f textFill=osg::Vec4f(95.0/255.0, 95.0 / 255.0, 95.0 / 255.0,1.0);
	//透明度
	bool lighting=false;
	bool depthTest=false;
	osgEarth::TextSymbol::Alignment textAlignment= osgEarth::TextSymbol::ALIGN_CENTER_TOP;
	osgEarth::IconSymbol::Alignment IconAlignment= osgEarth::IconSymbol::ALIGN_CENTER_BOTTOM;
	//宽度
	//double linewidth;
	//切片范围
	double minrange=0.1;
	//原始编码
	std::string textEncoding= "utf-8";
	std::string textField="name";
	int textSize=16;
	std::string textFont= "simhei.ttf";

	bool iconDeclutter=false;
	float iconScale=1.0;
private:
	std::vector<osg::ref_ptr<osgEarth::PlaceNode>> placenodes;
	osgEarth::MapNode* _root;
	std::vector<osg::Vec3d> coords;
	std::vector<std::string> texts;
	osg::ref_ptr<osg::LOD> lod;
};

