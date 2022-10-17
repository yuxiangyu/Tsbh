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
#include <osgEarth/Controls>
#include <osgEarth/Registry>
#include <osgEarth/Terrain>
using namespace osgEarth;

class TsbhTools_EXPORT MeasurePathAnalysis :public TsbhTools
{
public:
	MeasurePathAnalysis (osg::Group* group, MapNode* node);
	~MeasurePathAnalysis ();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point) ;

	void  onMouseMove(GeoPoint* Point) ;

	void buildModel();

	osg::ref_ptr<osg::Vec3Array> MeasurePathAnalysis::getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num);

	osg::Geode * drawEdges(osg::ref_ptr<osg::Vec3Array>  points);

	void renderCircleNode(osg::Vec3d worldP,QString lablel);

	void buildLabelModel(GeoPoint position,double dMinDistance,double dMaxDistance,double dMinHeight, double dMaxHeight);
private:

	std::vector<osg::Vec3d> positions;
	osg::ref_ptr<osgEarth::Feature> mFeature;
	osg::ref_ptr<osgEarth::FeatureNode> mFeatureNode;
	osg::ref_ptr<osgEarth::LineString> lineString;

private:


	std::vector<GeoPoint*> m_vecPoint;


};

