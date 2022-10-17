#include "MeasureAzimuth.h"
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>

MeasureAzimuth::MeasureAzimuth(osg::Group* group, MapNode* node) :TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	mFeature = NULL;
	//mNode = NULL;
}


MeasureAzimuth::~MeasureAzimuth()
{
	//mRoot->removeChild(mFeatureNode);
}

void  MeasureAzimuth::onLeftButton(GeoPoint* Point)
{
	
	if (pointNum == 0)
	{
		mpolygon = new osgEarth::Polygon();
		mpolygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		lineString = new osgEarth::LineString();
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		//lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));

		mFeature = new Feature(lineString, mapNode->getMapSRS()->getGeographicSRS());
		mFeature->geoInterp() = GEOINTERP_RHUMB_LINE;
		this->pointNum++;

		buildModel();
	}
	else
	{
		mpolygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		//setPoint(Point);
		lineString->at(lineString->size() - 1).x() = Point->x();
		lineString->at(lineString->size() - 1).y() = Point->y();
		lineString->at(lineString->size() - 1).z() = Point->z();
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		this->pointNum++;
		mFeatureNode->dirty();
		//updateModel();

		/*mRoot->addChild(createLabel());*/
	}
	renderCircleNode(Point);

	if(pointNum == 3)
	{
		m_bFinish = true;
		mRoot->addChild(createLabel());
	}
}

void  MeasureAzimuth::onDoubleClick(GeoPoint* Point)
{
	onLeftButton(Point);
	//mRoot->addChild(createLabel());
}

void  MeasureAzimuth::onMouseMove(GeoPoint* Point)
{
	lineString->at(lineString->size() - 1).x() = Point->x();
	lineString->at(lineString->size() - 1).y() = Point->y();
	lineString->at(lineString->size() - 1).z() = Point->z();
	mFeatureNode->dirty();
}

void MeasureAzimuth::buildModel()
{
	Style style;
	AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
	alt->technique() = AltitudeSymbol::TECHNIQUE_GPU;
	alt->verticalOffset() = 0.1;
	alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	RenderSymbol* render = style.getOrCreate<RenderSymbol>();
	render->minAlpha() = 0.15f;
	LineSymbol* ls = style.getOrCreate<LineSymbol>();

	ls->stroke()->color() = Color::Blue;

	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 20;

	//PointSymbol* ps = style.getOrCreate<PointSymbol>();
	//ps->size() = 2;

	//ps->fill()->color() = Color::Red;

	mFeatureNode = new FeatureNode(mFeature, style);
	mFeatureNode->dirty();

	mRoot->addChild(mFeatureNode);
}

osg::Node* MeasureAzimuth::createLabel()
{
	auto geom_feature = mFeature->getGeometry();
	if (geom_feature->size() < 3)
		return NULL;

	auto& center = geom_feature->at(1);
	auto& proj1 = geom_feature->at(0);
	auto& proj2 = geom_feature->at(2);
	auto radius = std::min((proj1 - center).length(), (proj2 - center).length()) / 4.0;
	osg::Vec3d p1 = center + (proj1 - center) / 4;
	osg::Vec3d p2 = center + (proj2 - center) / 4;
	GeoPoint p3(mapNode->getMapSRS(), (p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2, (p1.z() + p2.z()) / 2, osgEarth::ALTMODE_ABSOLUTE);
	GeoPoint geo_center(mapNode->getMapSRS(), center.x(), center.y(), osgEarth::ALTMODE_ABSOLUTE);
	GeoPoint geo_proj1(mapNode->getMapSRS(), proj1.x(), proj1.y(), osgEarth::ALTMODE_ABSOLUTE);
	GeoPoint geo_proj2(mapNode->getMapSRS(), proj2.x(), proj2.y(), osgEarth::ALTMODE_ABSOLUTE);
	using osgEarth::GeoMath;
	auto angle_start = osg::RadiansToDegrees(GeoMath::bearing(osg::DegreesToRadians(geo_proj1.y()), osg::DegreesToRadians(geo_proj1.x()), osg::DegreesToRadians(geo_center.y()), osg::DegreesToRadians(geo_center.x())));
	auto angle_end = osg::RadiansToDegrees(GeoMath::bearing(osg::DegreesToRadians(geo_proj2.y()), osg::DegreesToRadians(geo_proj2.x()), osg::DegreesToRadians(geo_center.y()), osg::DegreesToRadians(geo_center.x())));
	angle_start = angle_start < 0 ? angle_start + 360.0 : angle_start;
	angle_end = angle_end < 0 ? angle_end + 360.0 : angle_end;
	Style style;
	style.getOrCreate<BBoxSymbol>()->border()->width() = 2.0;
	style.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	style.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	style.getOrCreate<TextSymbol>()->size() = 18.0f;
	style.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	style.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	style.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	float area1 = 0.0f;
	float s = 3.1415926 / 180.0;
	float cx = 0.f, cy = 0.f, cz = 0.f;
	for (int i = 0; i < mpolygon->size(); i++)
	{
		osg::Vec3d p1 = mpolygon->at(i);
		osg::Vec3d p2 = mpolygon->at((i + 1) % mpolygon->size());
		//area1 += (p2.x() - p1.x())*s*(2 + sin(p1.y()*s) + sin(p2.y()*s));

		osg::Vec3d pp = mpolygon->at(i);
		cx += pp.x();
		cy += pp.y();
		cz += pp.z();
	}

	osg::Vec3d center1(cx / mpolygon->size(), cy / mpolygon->size(), cz / mpolygon->size());

	std::stringstream stream;
	stream.precision(2);
	stream.setf(std::ios::fixed);
	stream << u8"角度:" << angle_end - angle_start;

	LabelNode* labelNode = new LabelNode(GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), center1), stream.str(), style);


	return labelNode;
}

void MeasureAzimuth::renderCircleNode(GeoPoint* point)
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

	//mapPoint.createLocalToWorld(matrix);

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