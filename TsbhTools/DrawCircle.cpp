
#include "DrawCircle.h"
#include <osgEarth/MapNode>
#include <osgEarth/GeoMath>

DrawCircle::DrawCircle(osg::Group* group, osgEarth::MapNode* node) :TsbhTools()
			,m_StartAngle(0)
			,m_EndAngle(0)
{
	this->mapNode = node;
	this->mRoot = group;
	m_centerPoint = osg::Vec3d();
	m_CircleRadius = 0;

}

DrawCircle::~DrawCircle()
{
}

void DrawCircle::initCircle()
{
	mRoot->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	osg::ref_ptr<CircleNode> pCircleNode = NULL;

	m_circleStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(0, 0, 1, 0.5);// osgEarth::Color::Yellow;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	LineSymbol* ls = m_circleStyle.getOrCreate<LineSymbol>();
	ls->stroke()->color() = Color::Red;
	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 50;

	{
		pCircleNode = new osgEarth::CircleNode();

		pCircleNode->set(
			GeoPoint(mapNode->getMapSRS(), m_centerPoint, ALTMODE_ABSOLUTE),
			Distance(0, Units::KILOMETERS),
			m_circleStyle,
			Angle(m_StartAngle, Units::DEGREES),
			Angle(m_EndAngle, Units::DEGREES),
			true);
		
		mRoot->addChild(pCircleNode);
		//mRoot->addChild(m_pCircleEdit);
	}

	double radius = GeoMath::distance(osg::Vec3d(m_centerPoint.x(), m_centerPoint.y(), m_centerPoint.z()), osg::Vec3d(m_centerPoint.x()+ m_CircleRadius, m_centerPoint.y()+ m_CircleRadius, m_centerPoint.z()), mapNode->getMapSRS()->getGeographicSRS());
	pCircleNode->setRadius(Distance(radius, Units::METERS));
}

void DrawCircle::setCenterPoint(osg::Vec3d centerPoint, double radius)
{
	m_centerPoint = centerPoint;
	m_CircleRadius = radius;
}

void DrawCircle::setCircleAngle(double startangle,double endangle)
{
	m_StartAngle = startangle;
	m_EndAngle = endangle;
}