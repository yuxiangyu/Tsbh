#include "MeasureArea.h"
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>

MeasureArea::MeasureArea(osg::Group* group, MapNode* node) :TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	mFeature = NULL;
	mNode = NULL;
}


MeasureArea::~MeasureArea()
{
	//mRoot->removeChild(mFeatureNode);
}

void  MeasureArea::onLeftButton(GeoPoint* Point)
{
	addPoint(Point);

	renderCircleNode(Point);
}

void  MeasureArea::onDoubleClick(GeoPoint* Point)
{
	//if (pointNum == 1)
	//{
	//	 return;
	//}
	//addPoint(Point);
	//renderCircleNode(Point);
	this->m_bFinish = true;
}

void  MeasureArea::onMouseMove(GeoPoint* Point)
{
	setPoint(Point);
}

void MeasureArea::buildModel()
{
	Style style;
	style.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = Color(1 / 255.0, 255 / 255.0, 238 / 255.0, 100 / 255.0);
	AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
	alt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
	alt->verticalOffset() = 0.1;
	alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	RenderSymbol* render = style.getOrCreate<RenderSymbol>();
	render->minAlpha() = 0.15f;
	LineSymbol* ls = style.getOrCreate<LineSymbol>();

	ls->stroke()->color() = Color(8 / 255.0, 140 / 255.0, 255 / 255.0, 200 / 255.0);

	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 20;

	//PointSymbol* ps = style.getOrCreate<PointSymbol>();
	//ps->size() = 2;

	//ps->fill()->color() = Color::Red;

	mFeatureNode = new FeatureNode(mFeature, style);
	mFeatureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	mFeatureNode->dirty();

	mRoot->addChild(mFeatureNode);
}

osg::Node* MeasureArea::createLabel()
{
	Style style;
	style.getOrCreate<TextSymbol>()->size() = 18.0f;
	style.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	style.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	style.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	style.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	float area1 = 0.0f;
	float s = 3.1415926 / 180.0;
	float cx = 0.f, cy = 0.f, cz = 0.f;
	for (int i = 0; i < polygon->size(); i++)
	{
		osg::Vec3d p1 = polygon->at(i);
		osg::Vec3d p2 = polygon->at((i + 1) % polygon->size());
		area1 += (p2.x() - p1.x())*s*(2 + sin(p1.y()*s) + sin(p2.y()*s));

		osg::Vec3d pp = polygon->at(i);
		cx += pp.x();
		cy += pp.y();
		cz += pp.z();
	}

	osg::Vec3d center1(cx / polygon->size(), cy / polygon->size(), cz / polygon->size());

	//OE_WARN << area1 << std::endl;
	area1 = abs(6378136.49*area1*6378136.49 / 2);
	std::stringstream stream;
	stream.precision(2);
	stream.setf(std::ios::fixed);
	if (area1 > (1000 * 1000))
		stream << area1 / 1e6 << u8"平方千米";
	else
		stream << area1 << u8"平方米";
	LabelNode* labelNode = new LabelNode(GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), center1), stream.str(), style);


	return labelNode;

}

void MeasureArea::renderCircleNode(GeoPoint* point)
{
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f);
	osg::Geode* geode = new osg::Geode();
	osg::ShapeDrawable* _shapeDrawable = new osg::ShapeDrawable(shape);
	_shapeDrawable->setDataVariance(osg::Object::DYNAMIC);
	_shapeDrawable->setColor(Color(1, 0,0, 1));
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
void MeasureArea::addPoint(GeoPoint* Point)
{
	if (pointNum == 0)
	{
		polygon = new osgEarth::Polygon();
		polygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		pointNum++;
	}
	else if (pointNum == 1)
	{
		//polygon = new osgEarth::Annotation::Polygon();
		polygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		polygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));

		mFeature = new Feature(polygon, mapNode->getMapSRS()->getGeographicSRS());
		mFeature->geoInterp() = GEOINTERP_RHUMB_LINE;
		this->pointNum++;

		buildModel();
	}
	else
	{
		polygon->at(polygon->size() - 1).x() = Point->x();
		polygon->at(polygon->size() - 1).y() = Point->y();
		polygon->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		this->pointNum++;
		mFeatureNode->dirty();
		setLabel();
	}
}
void MeasureArea::setPoint(GeoPoint* Point)
{
	if (pointNum < 2)
		return;
	polygon->at(polygon->size() - 1).x() = Point->x();
	polygon->at(polygon->size() - 1).y() = Point->y();
	polygon->at(polygon->size() - 1).z() = Point->z();
	mFeatureNode->dirty();
}

std::vector<osg::Vec3d> MeasureArea::getPositions()
{
	std::vector<osg::Vec3d> positions;
	for (int i = 0; i < polygon->size(); i++)
	{
		osg::Vec3d position;
		position.x() = polygon->at(i).x();
		position.y() = polygon->at(i).y();
		position.z() = polygon->at(i).z();
		positions.push_back(position);
	}
	return positions;
}

void MeasureArea::setLabel()
{
	if (!mNode)
	{
		mNode = createLabel();
		mRoot->addChild(mNode);
	}
	else
	{
		mRoot->removeChild(mNode);
		mNode = createLabel();
		mRoot->addChild(mNode);

	}
}