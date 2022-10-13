#pragma once
#include "TsbhPlot_global.h"
#include <osgEarth/FeatureNode>
#include <osgEarth/Feature>
#include <osgEarth/Style>
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>

class TsbhPlot_EXPORT SameCenter
{
public:
	SameCenter(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
	SameCenter(osgEarth::MapNode* mapNode, osg::Group* drawGroup, double maxHeight);
	void draw(int CircleNum , double MaxRadius, osg::Vec3d center,int status,std::string name);
	void remove();
	void renderCircleNode();
	//status  1：未开始；2：部署中；3：执行中：4：已完成；5：已过期
	void setStatus(int status);

	void setId(std::string id) { m_Id = id; }
	std::string getId() { return m_Id; }
private:
	osgEarth::MapNode* _mapNode;
	//osg::Node::NodeMask _intersectionMask;
	osg::Group* _drawGroup;
	int m_iCircleNum;
	double m_dMaxRadius;
	osg::Vec3d m_center;
	int status;
	std::string name;
	double _maxHeight;
	int dd;
	osg::ref_ptr<osg::Group> group;
	
	osgEarth::Color color1;
	osgEarth::Style style;osgEarth::Style textstyle;
	osg::ref_ptr<osgEarth::LabelNode> labelNode;
	std::vector<osg::ref_ptr<osgEarth::FeatureNode> >m_vecFeatureNode;
	osg::ref_ptr<osg::ShapeDrawable> _shapeDrawable;
	std::string m_Id;
};