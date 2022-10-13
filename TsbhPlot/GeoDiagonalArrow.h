
#ifndef GEODIAGONALARROW_H
#define GEODIAGONALARROW_H

#include "DrawTool.h"
#include "featureEditing.h"
#include <osgEarth/FeatureNode>
#include <osgEarth/Style>

class TsbhPlot_EXPORT GeoDiagonalArrow : public DrawTool
{
public:
    GeoDiagonalArrow(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
    virtual DrawType getType() { return DRAW_DIAGONALARROW; }

    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove() {};		//用于移除已添加的节点 - add by gkk 20220810

    /**
     * @brief 只有两个控制点时
     * @param ctrlPts
     * @return
     */
	void setStyle(osgEarth::Style style);

    std::vector<osg::Vec2> calculateTwoPoints(const std::vector<osg::Vec2>& ctrlPts);
    /**
     * @brief 有三个或三个以上的控制点时
     * @param ctrlPts
     * @return
     */
    std::vector<osg::Vec2> calculateMorePoints(const std::vector<osg::Vec2>& ctrlPts);


private:
    osgEarth::Style _polygonStyle;
    osg::ref_ptr<osgEarth::FeatureNode>_featureNode;
    float _ratio;
};

#endif
