#pragma once
#include <osgEarth/PlaceNode>
#include <osgEarth/Style>
#include <osgDB\ReadFile>
#include "DrawTool.h"

class TsbhPlot_EXPORT DrawImageTool :
	public DrawTool
{
public:
	DrawImageTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
	~DrawImageTool();

	virtual DrawType getType() { return DRAW_IMAGE; }
	virtual ToolType getToolType() { return IMAGE; }
	virtual void beginDraw(const osg::Vec3d& lla);
	virtual void moveDraw(const osg::Vec3d& lla);
	virtual void endDraw(const osg::Vec3d& lla);
	virtual void resetDraw();
	virtual void remove();

	osg::ref_ptr<osg::Node> draw(const osg::Vec3d & lla);

	void setStyle(osgEarth::Style style);

	std::vector<osg::Vec3d> getPoints();

	void setImagePath(std::string image);

	NodeInfo getNodeinfos();
private:
	osg::Vec3d _centerPoint;

	std::vector<osg::Vec3d> m_temPoint;

	osgEarth::Style m_imageStyle;

	ToolProperty*  m_Property = NULL;

	std::string m_Iamgepath;

	osg::ref_ptr<osgEarth::PlaceNode> m_ImageNode;

	osgEarth::MapNode* m_mapNode;

	osg::ref_ptr<GeoPositionNodeEditor>m_geoeditr;
};

