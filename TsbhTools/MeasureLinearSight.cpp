#include "MeasureLinearSight.h"
#include <osgEarth/MapNode>
#include <osgEarth/LinearLineOfSight>
#include <osgUtil/IntersectVisitor>
#include <QString>
#include <osg/LineWidth>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/ImageStream>
#include <osg/Texture2D>
#include <osgEarth/ImageOverlay>
#include <osgEarth/Draggers>
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgSim/VisibilityGroup>
#include <osgSim/LineOfSight>
#include <osgEarth/LinearLineOfSight>

MeasureLinearSight::MeasureLinearSight(osg::Group* group, MapNode* node) :TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	position = NULL;
	pointNum = 0;
	mPolygon = NULL;

	this->mapNode = node;
	this->mRoot = group;
	m_pFeature = NULL;
	m_pFeatureNode = NULL;
	m_vec3dPoint.clear();
	m_vecPointDraw.clear();
	m_lineStyleYellow.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = Color(0, 0, 0, 0);
	m_lineStyleYellow.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = 2.0f;
	m_lineStyleYellow.getOrCreate<osgEarth::LineSymbol>()->tessellation() = 20.0;
	m_lineStyleYellow.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	m_lineStyleYellow.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	m_lineStyleYellow.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	m_lineStyleRed.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color::Red;
	m_lineStyleRed.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = 2.0f;
	m_lineStyleRed.getOrCreate<osgEarth::LineSymbol>()->tessellation() = 20.0;
	m_lineStyleRed.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	m_lineStyleRed.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	m_lineStyleRed.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	//mRoot->addChild(CreateSphere(osg::Vec3d(100,200, 220231)));
}

MeasureLinearSight::~MeasureLinearSight()
{
}

RichLabelNode* MeasureLinearSight::buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode)
{
	Style labelStyle;
	labelStyle.getOrCreate<osgEarth::TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";  // 暂时
	labelStyle.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_LEFT_CENTER;
	labelStyle.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	labelStyle.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(2, 100);

	osg::ref_ptr<RichLabelNode> label = new RichLabelNode(root, pos, text, labelStyle);
	label->setDynamic(true);
	return label;
}

std::string MeasureLinearSight::buildText(const GeoPoint &pos, const std::string& name)
{
	std::stringstream ss;
	ss.precision(2);
	ss.setf(std::ios::fixed);
	ss << "\n经度:" << pos.x() << std::endl
		<< "纬度:" << pos.y() << std::endl
		<< "海拔:" << pos.z() << std::endl;
	std::cout << name + ss.str() << std::endl;

	return name + ss.str();
}
void  MeasureLinearSight::onLeftButton(GeoPoint* Point)
{

	if (m_vec3dPoint.size() == 2)
	{
		osg::Vec3d point(Point->x(), Point->y(), Point->alt()+2);
		m_vec3dPoint[1] = point;

		//DrawLine(m_lineStyleYellow);
		analysis();
		buildLabel(*Point, buildText(*Point, "终点"), mRoot, mapNode);
		return;
	}
	else
	{
		m_vec3dPoint.push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()+2));
		if (m_vec3dPoint.size() == 2)
		{
			DrawLine(m_lineStyleYellow);
			analysis();
			buildLabel(*Point, buildText(*Point, "终点"), mRoot, mapNode);
			return;
		}
	}
	buildLabel(*Point, buildText(*Point, "起始点"), mRoot, mapNode);
}

void  MeasureLinearSight::onDoubleClick(GeoPoint* Point)
{

	if (m_vec3dPoint.size() == 2)
	{
		osg::Vec3d point(Point->x(), Point->y(), Point->alt()+2);
		m_vec3dPoint[1] = point;
		//DrawLine(m_lineStyleYellow);
		analysis();
	}
	else
	{
		m_vec3dPoint.push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()+2));
		if (m_vec3dPoint.size() == 2)
		{
			//DrawLine(m_lineStyleYellow);
			analysis();
		}
	}
	buildLabel(*Point, buildText(*Point, "终点"), mRoot, mapNode);

	this->m_bFinish = true;
}

void  MeasureLinearSight::onMouseMove(GeoPoint* Point)
{

}

void MeasureLinearSight::analysis()
{
	GeoPoint p1(mapNode->getMapSRS()->getGeographicSRS(), m_vec3dPoint.at(0), ALTMODE_ABSOLUTE);
	GeoPoint p2(mapNode->getMapSRS()->getGeographicSRS(), m_vec3dPoint.at(1), ALTMODE_ABSOLUTE);
	osgEarth::Contrib::LinearLineOfSightNode* los = new osgEarth::Contrib::LinearLineOfSightNode(mapNode, p1,p2);

	mRoot->addChild(los);

	/*osg::Vec3d startWorld;
	osg::Vec3d endWorld;
	mapNode->getMapSRS()->transformToWorld(osg::Vec3d(m_vec3dPoint[0].x(), m_vec3dPoint[0].y(), m_vec3dPoint[0].z()), startWorld);
	mapNode->getMapSRS()->transformToWorld(osg::Vec3d(m_vec3dPoint[1].x(), m_vec3dPoint[1].y(), m_vec3dPoint[1].z()), endWorld);

	osgSim::LineOfSight los;
	los.clear();
	los.addLOS(startWorld, endWorld);
	los.computeIntersections(mapNode);
	osg::Vec3d firstPoint(0, 0, 0);
	m_vecPointDraw.clear();
	const osgSim::LineOfSight::Intersections& intersections = los.getIntersections(0);
	for (osgSim::LineOfSight::Intersections::const_iterator itr = intersections.begin(); itr != intersections.end(); ++itr)
	{
		int a = intersections.size();
		osg::Vec3d point = *itr;
		double dis1 = GeoMath::distance(startWorld, point, mapNode->getMapSRS()->getGeographicSRS());
		double dis2 = GeoMath::distance(endWorld, point, mapNode->getMapSRS()->getGeographicSRS());

		if (fabs(startWorld.x() - point.x()) <= 1 && fabs(startWorld.y() - point.y()) <= 1)
			continue;
		if (fabs(endWorld.x() - point.x()) <= 1 && fabs(endWorld.y() - point.y()) <= 1)
			continue;
		if (firstPoint != point)
		{
			firstPoint = point;
		}
		else
		{
			continue;
		}
		
		osg::Vec3d cetPoint;
		mapNode->getMapSRS()->transformFromWorld(point, cetPoint);
		m_vecPointDraw.push_back(cetPoint);
		GeoPoint gpoint(mapNode->getMapSRS(), cetPoint);
		renderCircleNode(&gpoint);
		buildLabel(gpoint, buildText(gpoint, "交点"), mRoot, mapNode);
	}
	DrawLine(m_lineStyleRed,1);*/
}

osg::Vec3d MeasureLinearSight::getWorldCoord(osg::Vec3d pos)
{
	osg::Vec3d world;
	mapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(pos.y()),
		osg::DegreesToRadians(pos.x()),
		pos.z(), world.x(), world.y(), world.z());

	return world;
}
void MeasureLinearSight::DrawLine(const Style& style, bool redYellow)
{
	if (m_vec3dPoint.size() <= 1 && m_vecPointDraw.size()<= 1)
	{
		return;
	}
	osg::ref_ptr<osgEarth::Feature>	pFeature = new osgEarth::Feature(new osgEarth::LineString, mapNode->getMapSRS()->getGeographicSRS());
	osg::ref_ptr<osgEarth::FeatureNode> pFeatureNode = new osgEarth::FeatureNode(pFeature, style);
	pFeatureNode->setStyle(style);
	if (!redYellow)
	{
		for (auto& n : m_vec3dPoint)
		{
			//n[2] = 1000;//预设高度1000
			pFeature->getGeometry()->push_back(n);
		}
	}
	else
	{
		for (auto& n : m_vecPointDraw)
		{
			//n[2] = 1000;//预设高度1000
			pFeature->getGeometry()->push_back(n);
		}
	}

	pFeatureNode->dirty();
	mRoot->addChild(pFeatureNode);
}

osg::Vec3d MeasureLinearSight::LonLatAltToWorld(const osg::Vec3d lonLatAlt)
{
	osg::Vec3d world;
	osg::EllipsoidModel* em = new osg::EllipsoidModel();
	//Lat纬度Long经度
	em->convertLatLongHeightToXYZ(osg::DegreesToRadians(lonLatAlt.y()),
		osg::DegreesToRadians(lonLatAlt.x()), lonLatAlt.z(), world.x(), world.y(), world.z());
	return world;
}
osg::Vec3d MeasureLinearSight::WorldToLonLatAlt(const osg::Vec3d world)
{
	osg::EllipsoidModel* em = new osg::EllipsoidModel();
	osg::Vec3d lonLatAlt;
	em->convertXYZToLatLongHeight(world.x(), world.y(), world.z(),
		lonLatAlt.y(), lonLatAlt.x(), lonLatAlt.z());

	lonLatAlt.x() = osg::RadiansToDegrees(lonLatAlt.x());
	lonLatAlt.y() = osg::RadiansToDegrees(lonLatAlt.y());
	return lonLatAlt;
}
void MeasureLinearSight::renderCircleNode(GeoPoint* point)
{
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f);
	osg::Geode* geode = new osg::Geode();
	osg::ShapeDrawable* _shapeDrawable = new osg::ShapeDrawable(shape);
	_shapeDrawable->setDataVariance(osg::Object::DYNAMIC);
	_shapeDrawable->setColor(Color(1, 0, 0, 1));
	geode->addDrawable(_shapeDrawable);

	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::Vec3d worldP;
	point->toWorld(worldP);

	osg::MatrixTransform* _scaler = new osg::MatrixTransform;
	_scaler->setMatrix(osg::Matrixd::scale(5, 5, 5));
	_scaler->addChild(geode);

	osg::AutoTransform* at = new osg::AutoTransform;
	at->setAutoScaleToScreen(true);
	at->addChild(_scaler);

	osg::MatrixTransform* _transfer = new osg::MatrixTransform;
	_transfer->setMatrix(osg::Matrixd::translate(worldP.x(), worldP.y(), worldP.z()));
	_transfer->addChild(at);
	mRoot->addChild(_transfer);
}