
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
	virtual void remove() {};		//�����Ƴ�����ӵĽڵ� - add by gkk 20220810

    /**
     * @brief ֻ���������Ƶ�ʱ
     * @param ctrlPts
     * @return
     */
	void setStyle(osgEarth::Style style);

    std::vector<osg::Vec2> calculateTwoPoints(const std::vector<osg::Vec2>& ctrlPts);
    /**
     * @brief ���������������ϵĿ��Ƶ�ʱ
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
