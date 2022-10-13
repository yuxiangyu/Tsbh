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

	//�������
	int type;
	//��ͼ����,0Ϊ3ά��1Ϊ2ά
	int viewtype;
	//�������
	string plotname;
	//������ɫ
	osg::Vec4 color;
	//���ɫ
	osg::Vec4 color2;
	//���С���߿�
	float width;
	//��λ
	std::vector<osg::Vec2> _controlPoints;
};
