#pragma once
#include <osgEarth/GeoPositionNode>
#include <osgEarth/CircleNode>
#include <osgEarth/Style>
#include <osgDB\ReadFile>
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>
#include "DrawTool.h"
#include "AnnotationEditing.h"

class TsbhPlot_EXPORT DrawGeoPoint :
	public DrawTool
{
public:
	DrawGeoPoint(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
	~DrawGeoPoint();

	virtual DrawType getType() { return DRAW_POSITION; }
	virtual ToolType getToolType() { return GEOPOSITION; }
	virtual void beginDraw(const osg::Vec3d& lla);
	virtual void moveDraw(const osg::Vec3d& lla);
	virtual void endDraw(const osg::Vec3d& lla);
	virtual void resetDraw();
	virtual void remove();

	osg::ref_ptr<osg::Node> draw(const osg::Vec3d & lla);

	void setStyle(osgEarth::Style style);
	
	osg::ref_ptr<osg::MatrixTransform> renderCircleNode(GeoPoint* point);

	std::vector<osg::Vec3d> getPoints();

	NodeInfo getNodeinfos();
private:
	osg::Vec3d _centerPoint;

	std::vector<osg::Vec3d> m_temPoint;

	osgEarth::Style m_GeoPointStyle;

	ToolProperty*  m_Property = NULL;

	osg::ref_ptr<osgEarth::GeoPositionNode> m_GeoPositionNode;

	osg::ref_ptr<osgEarth::CircleNode> _circleNode;
	osg::ref_ptr<GeoPositionNodeEditor> _circleEdit;

	osg::ref_ptr<osg::MatrixTransform> m_posNode;

	osgEarth::MapNode* m_mapNode;

	osg::ref_ptr<GeoPositionNodeEditor>m_geoeditr;
};
