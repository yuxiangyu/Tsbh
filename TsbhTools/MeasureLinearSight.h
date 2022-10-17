#pragma once
#include "RichLabelNode.h"
#include "TsbhTools.h"
#include <osgEarth/MapNode>
#include <osgEarth/Style>
#include <osgEarth/TextSymbol>
#include <osgEarth/BBoxSymbol>
#include <osgEarth\PlaceNode>


using namespace osgEarth;

class TsbhTools_EXPORT MeasureLinearSight :public TsbhTools
{
public:
	MeasureLinearSight(osg::Group* group, MapNode* node);
	~MeasureLinearSight();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point);

	void  onMouseMove(GeoPoint* Point);

	RichLabelNode* buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode);
	std::string buildText(const GeoPoint &pos, const std::string& name);
	void DrawLine(const Style& style,bool redYellow = false);
	void renderCircleNode(GeoPoint* point);
	osg::Vec3d LonLatAltToWorld(const osg::Vec3d lonLatAlt);
	osg::Vec3d WorldToLonLatAlt(const osg::Vec3d world);
	void analysis();
	osg::Vec3d getWorldCoord(osg::Vec3d pos);
private:

	GeoPoint* position;
	RichLabelNode* mLabelNode;
	osg::ref_ptr<osgEarth::Feature> m_pFeature;
	osg::ref_ptr<osgEarth::FeatureNode> m_pFeatureNode;
	osg::ref_ptr<osgEarth::Polygon> mPolygon;
	int pointNum;

private:
	osgEarth::FeatureNode* m_pLine;
	osg::Vec3d m_firstPoint;
	osgEarth::PlaceNode* m_pPlaceNode;
	osgEarth::Style m_lineStyleYellow;
	osgEarth::Style m_lineStyleRed;
	std::vector<osg::Vec3d> m_vec3dPoint;
	std::vector<osg::Vec3d> m_vecPointDraw;

	GeoPoint* m_PoinStart;
	GeoPoint* m_PointEnd;
	bool m_bStart = false;
};