#pragma once

#include "RichLabelNode.h"
#include "TsbhTools.h"

#include <osgEarth/MapNode>
#include <osgEarth/Style>
#include <osgEarth/TextSymbol>
#include <osgEarth/BBoxSymbol>
#include "osgEarth/Style"
#include <osgEarth/CircleNode>
#include <osgEarth/AnnotationUtils>
#include <osgEarth/AnnotationRegistry>

using namespace osgEarth;

class TsbhTools_EXPORT DrawCircle :public TsbhTools
{
public:
	DrawCircle(osg::Group* group, osgEarth::MapNode* node);
	~DrawCircle();

	void setCircleAngle(double startangle, double endangle);
	void setCenterPoint(osg::Vec3d centerPoint,double radius);
	void initCircle();
private:
	osg::Vec3d m_centerPoint;
	double m_CircleRadius;
	osgEarth::Style m_circleStyle;
	double m_StartAngle;
	double m_EndAngle;

};



