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
#include <osgEarth/PlaceNode>
#include <osgEarth/DrapeableNode>
#include <osgEarth/RadialLineOfSight>
#include "RadialLineOfSight.h"
using namespace osgEarth;

class TsbhTools_EXPORT MeasureRadialAnalysis:public TsbhTools
{
public:
	MeasureRadialAnalysis(osg::Group* group, MapNode* node);
	~MeasureRadialAnalysis();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point) ;

	void  onMouseMove(GeoPoint* Point) ;

	void buildModel();
	osg::Node* createLabel();
	void renderCircleNode(GeoPoint*);

	//osg::ref_ptr<osg::Geode> CreateLine(const osg::Vec3 &start, const osg::Vec3 &end);

	void cereatLine();
private:

	std::vector<osg::Vec3d> positions;
	osg::ref_ptr<osgEarth::Feature> mFeature;

	osg::ref_ptr<osgEarth::FeatureNode> mFeatureNode;
	osg::ref_ptr<osgEarth::LineString> lineString;

private:
	//osgEarth::Style m_lineStyle;
	osgEarth::Feature* m_pFeature;
	osgEarth::FeatureNode* m_pFeatureNode;

	//osgEarth::Style m_stippleLineStyle;
	osgEarth::Feature* m_pStippleFeature;
	osgEarth::FeatureNode* m_pStippleFeatureNode;

	std::vector<osg::Vec3d> m_vecPoint;

	osg::Vec3d startline;
	RadialLineOfSightNode2* m_tetheredRadial = nullptr;
	osg::ref_ptr<osg::Group> DrapeableNode;
	//osg::ref_ptr<osgEarth::DrapeableNode> m_group;
};

