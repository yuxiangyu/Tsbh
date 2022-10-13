#ifndef GEOSTRAIGHTARROW_H
#define GEOSTRAIGHTARROW_H

#include "DrawTool.h"
#include "FeatureEditing.h"
#include <osgEarth/FeatureNode>
#include <osgEarth/Style>

class TsbhPlot_EXPORT GeoStraightArrow : public DrawTool {
public:
    GeoStraightArrow(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_STRAIGHTARROW; }

    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove() {};		//�����Ƴ�����ӵĽڵ� - add by gkk 20220810

private:
    /**
     * �����������Ƶ�ʱֱ��ͷ�����л��Ƶ�
     * �������Ƶ��ֱ��ͷ���Ƶ�ֻ��Ҫ7���Ϳ��Թ���
     */
    std::vector<osg::Vec2> calculateTwoPoints(const std::vector<osg::Vec2>& ctrlPts);
    /**
     * �����������������ϵĿ��Ƶ�ʱ�����л��Ƶ�
     * �����м�Ŀ��Ƶ�֮�����в�ֵ����������Ч��������������Ƶ��ܶ�
     * ����ʹ�õ�˼���ǽ������û����Ƶ����������γ�һ�����߶Σ�
     * ÿһ���߶�������������������ƽ���ߣ��������γ���һ��������ʽ�ļ�ͷ��
     * Ȼ���ڹսǽ������߻��������α��������߲�ֵ�������γ���Ч���ȽϺõļ�ͷ
     */
    std::vector<osg::Vec2> calculateMorePoints(const std::vector<osg::Vec2>& ctrlPts);

    /**
     * ��д�˸���ķ���
     * ����ͨ�����Ƶ�����ͷ�����л��Ƶ�
     */
    void calculateParts();

private:
    float _ratio;
    osgEarth::Style _polygonStyle;
    osg::ref_ptr<osgEarth::FeatureNode>_featureNode;
    osg::ref_ptr<FeatureEditor> _polygonEdit;
};

#endif
