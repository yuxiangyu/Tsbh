#ifndef DRAW_RECTANGLE_TOOL_H
#define DRAW_RECTANGLE_TOOL_H 1

#include <osgEarth/RectangleNode>
#include "AnnotationEditing.h"
#include "DrawTool.h"

// 矩形绘制工具
class TsbhPlot_EXPORT DrawRectangleTool : public DrawTool {
public:
    DrawRectangleTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_RECTANGLE; }
	virtual ToolType getToolType() { return RECTANGLE; }
    virtual void beginDraw(const osg::Vec3d& v);
    virtual void moveDraw(const osg::Vec3d& v);
    virtual void endDraw(const osg::Vec3d& v);
    virtual void resetDraw();
	virtual void remove();
	osg::ref_ptr<osg::Node> draw(std::vector<osg::Vec3d>points);

	void setStyle(osgEarth::Style style);

	std::vector<osg::Vec3d> getPoints() {
		return _temPoint;
	}

	NodeInfo getNodeinfos();

    // 判断点在矩形所在角位置
    osgEarth::RectangleNode::Corner getPointCorner(const osgEarth::GeoPoint& corner);

private:
    std::vector<osg::Vec3d> _vecPoints;
	std::vector<osg::Vec3d> _temPoint;
	ToolProperty*  m_Property = NULL;
    osgEarth::Style _rectangleStyle;
    osgEarth::RectangleNode* _rectangleNode;
	osg::ref_ptr<RectangleNodeEditor> m_editr;
};

#endif
