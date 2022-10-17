#pragma once
#include <osgEarth/CircleNode>
#include <osgEarth/AnnotationUtils>
#include <osgEarth/AnnotationRegistry>
#include <osgEarth/MapNode>
#include <osgEarth/Style>
#include <osgEarth/TextSymbol>
#include <osgEarth/BBoxSymbol>
#include <iostream>
#include "RichLabelNode.h"
#include "TsbhTools.h"
#include "osgEarth/Style"
#include "osgEarth/PlaceNode"
#include <QString>

#define MAXPOLYGONCOUNT 200
using namespace osgEarth;

typedef struct SGeometry_
{
	std::string id;
	int type;
	SGeometry_()
	{
		id = "";
		type = 0;
	}

	bool operator < (const SGeometry_ &a) const
	{
		return id < a.id;
	}
}SGeometry;

class TsbhTools_EXPORT DrawGeometry 
{
public:
	DrawGeometry(osg::LOD* group, osgEarth::MapNode* node);
	~DrawGeometry();

	void setCircleAngle(double startangle, double endangle);
	void setCircleCenterPoint(osg::Vec3d centerPoint,double radius);
	void setNameAndIcon(QString sName, int Type);
	void initCircle();
	void addPolygonPoint(double x,double y, double z,int index);
	Style setGeometryStyle();
	void drawPolygon(int index,osg::Vec3d vec = osg::Vec3d(0,0,0));
	void clearPolygon(int index);

	void setSubtasksId(std::string id);
	void addGeometry(SGeometry gtyKey,void *parameter);
	void removeGeometry(SGeometry gtyKey);

	void setGeometryVisible(std::string index,bool state,int type=1);//多边形的显示与隐藏

private:
	osg::Vec3d m_centerPoint;
	double m_circleRadius;
	osgEarth::Style m_circleStyle;
	double m_startAngle;
	double m_endAngle;
	QString m_sName;
	int m_nType;

	osg::ref_ptr<osgEarth::MapNode> m_mapNode;
	osg::ref_ptr<osg::LOD> m_root;
	osg::ref_ptr<osgEarth::Polygon> m_polygon[MAXPOLYGONCOUNT];

	std::map<SGeometry, void *> m_geometryPtr;
	SGeometry m_sSubtasks;
	osgEarth::Style m_pin;				//文字标注

	std::map<int, std::string> m_mapTypeImg;
};



