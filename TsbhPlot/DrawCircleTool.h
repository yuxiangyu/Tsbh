#ifndef DRAWCIRCLE_TOOL_H
#define DRAWCIRCLE_TOOL_H 1

#include <osgEarth/CircleNode>
#include <osgEarth/PlaceNode>
#include <osgEarth/Style>
#include "DrawTool.h"
#include "AnnotationEditing.h"

class TsbhPlot_EXPORT DrawCircleTool : public DrawTool {
public:
    DrawCircleTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_CIRCLE; }

	virtual ToolType getToolType() { return CIRCLE; }

    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove();
	osg::ref_ptr<osg::Node> draw(osg::Vec3d cneterPoint,osg::Vec3d distancepoint);

	void setStyle(osgEarth::Style style);

	std::vector<osg::Vec3d> getPoints();

	

	NodeInfo getNodeinfos();

private:
    osg::Vec3d _centerPoint;
	std::vector<osg::Vec3d> _temPoint;
    osgEarth::Style _circleStyle;
	ToolProperty*  m_Property = NULL;
    osg::ref_ptr<osgEarth::CircleNode> _circleNode;
    osg::ref_ptr<CircleNodeEditor> _circleEdit;
    osg::ref_ptr<osgEarth::PlaceNode> _radiusNode;
	osgEarth::MapNode* m_mapNode;
};
#endif
