#ifndef DRAW_EIGHT_TOOL_H
#define DRAW_EIGHT_TOOL_H 1

#include <osgEarth/RectangleNode>
#include <osgEarth/CircleNode>
#include "AnnotationEditing.h"
#include "DrawTool.h"

// 矩形绘制工具
class TsbhPlot_EXPORT DrawEightTool : public DrawTool {
public:
	DrawEightTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_RUNWAY; }
	virtual ToolType getToolType() { return RUNWAY; }
    virtual void beginDraw(const osg::Vec3d& v);
    virtual void moveDraw(const osg::Vec3d& v);
    virtual void endDraw(const osg::Vec3d& v);
    virtual void resetDraw();
	virtual void remove();
	osg::ref_ptr<osg::Node> draw(std::vector<osg::Vec3d>points);

	void setStyle(osgEarth::Style style);
	//获取长轴半径
	double getRadiasMax();
	//获取短轴半径
	double getRadiasMin();
	//获取角度
	double getAngle();
	//获取中点
	void buildConerPoint();

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
   // osgEarth::Style _rectangleStyle;
    osgEarth::RectangleNode* _rectangleNode;
	osg::ref_ptr<RectangleNodeEditor> m_editr;

	//中间的矩形
	//osgEarth::RectangleNode* _rectangleNode2;
	//首尾两个椭圆
	osg::ref_ptr<osgEarth::EllipseNode> _circleNode1 ;
	osg::ref_ptr<osgEarth::EllipseNode> _circleNode2 ;

	osgEarth::Style _rectangleStyle2;
	//osgEarth::RectangleNode* _rectangleNode2;
	//两个半圆
	osgEarth::Style _circleStyle1;
	//osgEarth::Style _circleStyle2;

	double width ;						//矩形的长度
	double heigth ;
};

#endif
