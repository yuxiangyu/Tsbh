#ifndef GEOGATHERINGPLACE_H
#define GEOGATHERINGPLACE_H

#include "DrawTool.h"
#include <osgEarth/FeatureNode>
#include <osgEarth/Feature>
#include <osgEarth/Style>
#include "FeatureEditing.h"

class TsbhPlot_EXPORT GeoGatheringPlace : public DrawTool {
public:

    GeoGatheringPlace(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_GATHERINGPLACE; }
	virtual ToolType getToolType() { return GEOGETHER; }
    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove();		//用于移除已添加的节点 - add by gkk 20220810
	osg::ref_ptr<osg::Node> draw(std::vector<osg::Vec2> points);

    /**
    * Method: calculateParts
    * 重写了父类的方法
    * 用于通过控制点计算聚集地符号的所有点
    */


    std::vector<osg::Vec2> calculateParts(const std::vector<osg::Vec2>& controlPoints);

	NodeInfo getNodeinfos();

private:
    osgEarth::Style _polygonStyle;
    osg::ref_ptr<osgEarth::FeatureNode> _featureNode = nullptr;
	osg::ref_ptr<FeatureEditor> _polygonEdit;
	ToolProperty*  m_Property = NULL;

	std::vector<osg::Vec2> m_ctrlPts;
};

#endif
