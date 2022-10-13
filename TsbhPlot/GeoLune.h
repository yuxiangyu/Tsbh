#pragma once

#include "DrawTool.h"
#include <osgEarth/Style>
#include <osgEarth/FeatureNode>
#include <osgEarth/Feature>
#include <osgEarth/Style>

class TsbhPlot_EXPORT GeoLune : public DrawTool {
public:
    GeoLune(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
    virtual DrawType getType() { return DRAW_GEOLUNE; }
    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	void draw(std::vector<osg::Vec3d> points);

    std::vector<osg::Vec2> calculateParts(const std::vector<osg::Vec2>& ctrlPts);

private:
    osgEarth::Style _polygonStyle;
	osg::ref_ptr<osgEarth::FeatureNode>_featureNode;
    /**
    * APIProperty: sides
    * {Integer} 弓形上圆弧的点密度。默认为720，即每隔1°绘制两个点。
    */
    int _sides;
};