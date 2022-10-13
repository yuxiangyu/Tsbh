#ifndef DRAWLINETOOL_H
#define DRAWLINETOOL_H 1

#include <osgEarth/FeatureNode>
#include <osgEarth/Feature>
#include <osgEarth/Style>
#include"FeatureEditing.h"
#include "DrawTool.h"

class TsbhPlot_EXPORT DrawLineTool : public DrawTool {
public:
    DrawLineTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

	//��ȡ�����ļ���Ĭ����ʽ
	void  ReadIniStyle();
	//��ȡ�������
    virtual DrawType getType() { return DRAW_LINE; }
	//���ù�������
	virtual ToolType getToolType() { return LINE; }

    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove();

	void setStartPoint(const osg::Vec3d& lla);

	osg::ref_ptr<osg::Node> draw(std::vector<osg::Vec3d> points);

	void setStyle(osgEarth::Style style);

	NodeInfo getNodeinfos();

	std::vector<osg::Vec3d> getPoints();

	
private:
    osgEarth::Style _lineStyle;   //�ߵ���ʽ
    osg::ref_ptr<osgEarth::Feature> _feature;
    osgEarth::Style _stippleLineStyle; //�����ߵ���ʽ
    osg::ref_ptr<osgEarth::Feature> _stippleFeature;
    osg::ref_ptr<osgEarth::FeatureNode> _stippleFeatureNode;
	ToolProperty*  m_Property = NULL;
    std::vector<osg::Vec3d> _vecPoint;
	std::vector<osg::Vec3d> _temPoint;
	osg::ref_ptr<osgEarth::FeatureNode> _featureNode;
	osg::ref_ptr<FeatureEditor> _featuredit;

};

#endif
