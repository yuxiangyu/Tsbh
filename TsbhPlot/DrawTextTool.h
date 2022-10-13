#pragma once
#include <osgEarth/PlaceNode>
#include <osgEarth/Feature>
#include <osgEarth/Style>
#include "DrawTool.h"
#include "AnnotationEditing.h"

class TsbhPlot_EXPORT DrawTextTool :
	public DrawTool
{
public:
	DrawTextTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
	~DrawTextTool();

	virtual DrawType getType() { return DRAW_TEXT; }
	virtual ToolType getToolType() { return TEXT; }
	virtual void beginDraw(const osg::Vec3d& lla);
	virtual void moveDraw(const osg::Vec3d& lla);
	virtual void endDraw(const osg::Vec3d& lla);
	virtual void resetDraw();
	virtual void remove();

	osg::ref_ptr<osg::Node> draw(osg::Vec3d cneterPoint);

	void setStyle(osgEarth::Style style);

	std::vector<osg::Vec3d> getPoints();

	void setString(std::string stream)
	{
		m_Text = stream;
	}

	NodeInfo getNodeinfos();
private:
	osgEarth::Style m_TextStyle;
	osg::ref_ptr<osgEarth::Feature> _feature;
	osgEarth::Style _stippleLineStyle;
	osg::ref_ptr<osgEarth::Feature> _stippleFeature;
	osg::ref_ptr<osgEarth::PlaceNode> m_stippleLabelNode;
	ToolProperty*  m_Property = NULL;
	osg::Vec3d _centerPoint;
	std::vector<osg::Vec3d> _temPoint;
	osg::ref_ptr<osgEarth::PlaceNode> m_labelNode;
	std::stringstream m_stream ;
	std::string m_Text;
	osgEarth::MapNode* m_mapNode;
	osg::ref_ptr<GeoPositionNodeEditor>m_geoeditr;
};

