#pragma once

#include <osgEarth/MapNode>
#include <osgEarth/FeatureNode>
#include <QMap>
#include <QString>
#include "TsbhPlot_global.h"
#include <osgEarth/PlaceNode>

using namespace std;

class TsbhPlot_EXPORT PlotStyle
{
public:
	PlotStyle() { type = 0; viewtype = 0; };
	osgEarth::Style buildStyle2(osg::Vec4 &color, osg::Vec4 &color2, float width, bool is);

	//标绘类型
	int type;
	//视图类型,0为3维，1为2维
	int viewtype;
	//标绘名称
	string plotname;
	//点线颜色
	osg::Vec4 color;
	//填充色
	osg::Vec4 color2;
	//点大小，线宽
	float width;
	//点位
	std::vector<osg::Vec2> _controlPoints;
};
