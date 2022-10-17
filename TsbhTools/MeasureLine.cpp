#include "MeasureLine.h"
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>


MeasureLine::MeasureLine(osg::Group* group, MapNode* node):TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;



	mFeature = NULL;
}


MeasureLine::~MeasureLine()
{
	//mRoot->removeChild(mFeatureNode);
}

void  MeasureLine::onLeftButton(GeoPoint* Point)
{
	if (pointNum == 0)
	{
		//lineString.clear();
		lineString = new osgEarth::LineString();
		//lineString->pu
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
		//setPoint(Point);
		lineString->at(lineString->size() - 1).x() = Point->x();
		lineString->at(lineString->size() - 1).y() = Point->y();
		lineString->at(lineString->size() - 1).z() = Point->z();
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		this->pointNum++;
		mFeatureNode->dirty();
		//updateModel();
		
			mRoot->addChild(createLabel());
	
	}
	renderCircleNode(Point);
}

void  MeasureLine::onDoubleClick(GeoPoint* Point)
{
	onLeftButton(Point);
	this->m_bFinish = true;
}

void  MeasureLine::onMouseMove(GeoPoint* Point)
{
	lineString->at(lineString->size() - 1).x() = Point->x();
	lineString->at(lineString->size() - 1).y() = Point->y();
	lineString->at(lineString->size() - 1).z() = Point->z();
	mFeatureNode->dirty();
}

void MeasureLine::buildModel()
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

osg::Node* MeasureLine::createLabel()
{
	Style style;
	
	//style.getOrCreate<BBoxSymbol>()->geom() = BBoxSymbol::BboxGeom::GEOM_BOX_ORIENTED;
	style.getOrCreate<BBoxSymbol>()->border()->width() = 2.0;
	style.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	style.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	style.getOrCreate<TextSymbol>()->size() = 18.0f;
	style.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	style.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	style.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	
	//style.getOrCreate<BBoxSymbol>()->border()->widthUnits()= Units::PIXELS;
	osg::Vec3d point;
	point.x() = (lineString->at(lineString->size() - 2).x() + lineString->at(lineString->size() - 3).x()) / 2;
	point.y() = (lineString->at(lineString->size() - 2).y() + lineString->at(lineString->size() - 3).y()) / 2;
	point.z() = (lineString->at(lineString->size() - 2).z() + lineString->at(lineString->size() - 3).z()) / 2;;
	GeoPoint currentPoint(mapNode->getMapSRS()->getGeographicSRS(), lineString->at(lineString->size() - 2));
	GeoPoint lastGeoPoint(mapNode->getMapSRS()->getGeographicSRS(), lineString->at(lineString->size() - 3));
	auto distance = currentPoint.distanceTo(lastGeoPoint);
	std::stringstream stream;
	stream.precision(2);
	stream.setf(std::ios::fixed);
	if (distance > 1000)
		stream << distance / 1e3 << u8"วงรื";
	else
		stream << distance << u8"รื";
	LabelNode* label= new LabelNode( stream.str(), style);
	label->setPosition(GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), point.x(), point.y(), point.z()));
	
	return label;
}

void MeasureLine::renderCircleNode(GeoPoint* point)
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
