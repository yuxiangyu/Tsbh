#pragma once
#include "TsbhTools_global.h"
#include "TsbhTools.h"
#include <osg/MatrixTransform>
#include <osg/Referenced>
#include <osg/NodeCallback>
#include <osg/Vec3>
#include <osgEarth/MapNode>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
using namespace osgEarth;

class TsbhTools_EXPORT MeasureLine:public TsbhTools
{
public:
	MeasureLine(osg::Group* group, MapNode* node);
	~MeasureLine();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point) ;

	void  onMouseMove(GeoPoint* Point) ;

	void buildModel();
	osg::Node* createLabel();
	void renderCircleNode(GeoPoint*);

private:

	std::vector<osg::Vec3d> positions;
	osg::ref_ptr<osgEarth::Feature> mFeature;

	osg::ref_ptr<osgEarth::FeatureNode> mFeatureNode;
	osg::ref_ptr<osgEarth::LineString> lineString;
};

