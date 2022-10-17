#pragma once
#include "TsbhTools_global.h"
#include "TsbhTools.h"
#include <osg/MatrixTransform>
#include <osg/Referenced>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Vec3>
#include <osgEarth/MapNode>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
using namespace osgEarth;

class TsbhTools_EXPORT MeasureArea :public TsbhTools
{
public:
	MeasureArea(osg::Group* group, MapNode* node);
	~MeasureArea();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point);

	void  onMouseMove(GeoPoint* Point);

	void buildModel();
	osg::Node* createLabel();
	void renderCircleNode(GeoPoint*);

private:
	osg::ref_ptr<osgEarth::MapNode> mapNode;

	std::vector<osg::Vec3d> positions;
	osg::ref_ptr<osgEarth::Feature> mFeature;

	osg::ref_ptr<osgEarth::FeatureNode> mFeatureNode;
	osg::ref_ptr<osgEarth::LineString> lineString;

	osgEarth::Polygon *polygon;

	osg::Node* mNode;

public:
	virtual void addPoint(GeoPoint* Point);
	virtual void setPoint(GeoPoint* Point);
	virtual std::vector<osg::Vec3d> getPositions();
	//virtual void setType2(int type2);
	void setLabel();
};