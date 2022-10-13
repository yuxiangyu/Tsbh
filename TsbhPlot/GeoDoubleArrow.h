#ifndef GEODOUBLEARROW_H
#define GEODOUBLEARROW_H

#include "DrawTool.h"
#include "FeatureEditing.h"
#include <osgEarth/FeatureNode>
#include <osgEarth/Style>

class TsbhPlot_EXPORT GeoDoubleArrow : public DrawTool {
public:
    GeoDoubleArrow(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
    virtual DrawType getType() { return DRAW_DOUBLEARROW; }

    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove() {};		//用于移除已添加的节点 - add by gkk 20220810

    std::vector<osg::Vec2> calculateParts(const std::vector<osg::Vec2>& ctrlPts);

private:
    osgEarth::Style _polygonStyle;
    osg::ref_ptr<osgEarth::FeatureNode>_featureNode;
};

#endif
