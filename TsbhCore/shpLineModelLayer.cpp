#include "shpLineModelLayer.h"
#include <osgEarth/OGRFeatureSource>
#include <osgEarth/FeatureNode>

using namespace osgEarth;

shpLineModelLayer::shpLineModelLayer() :zeVectorLayer()
{
}

shpLineModelLayer::shpLineModelLayer(std::string name, std::string filePath) : zeVectorLayer(name)
{
	//读取shp文件并构建placenode集合
	osg::ref_ptr< OGRFeatureSource > indexFeatures = new OGRFeatureSource;
	indexFeatures->setURL(filePath);
	if (indexFeatures->open().isError())
	{
		OE_WARN << "Failed to open index " << index << ": " << indexFeatures->getStatus().toString() << std::endl;
		return;
	}

	osg::ref_ptr< FeatureCursor > cursor = indexFeatures->createFeatureCursor(0);
	cursor->fill(listFeature);
	lod = new osg::LOD;
}

shpLineModelLayer::~shpLineModelLayer()
{
	listFeature.clear();
}

void shpLineModelLayer::addToScene(osgEarth::MapNode* root)
{
	const SpatialReference* geoSRS = root->getMapSRS()->getGeographicSRS();
	_root = root;
	if (lod)
		_root->addChild(lod);
	osgEarth::Style style;
	osgEarth::LineSymbol* ls = style.getOrCreateSymbol<osgEarth::LineSymbol>();
	ls->stroke()->color() = lineColor;
	ls->stroke()->width() = lineWidth;

	osgEarth::RenderSymbol* rs = style.getOrCreate<osgEarth::RenderSymbol>();
	rs->depthOffset()->enabled() = true;
	rs->lighting() = false;
	//rs->depthOffset()->minBias() = Distance(200.0, Units::KILOMETERS);
	osgEarth::AltitudeSymbol* alt = style.getOrCreate<osgEarth::AltitudeSymbol>();
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	osg::ref_ptr< FeatureNode > featureNode = new FeatureNode(listFeature, style);
	lod->addChild(featureNode.get(), this->minrange, this->maxrange);
}

void shpLineModelLayer::remove()
{
	if (_root)
	{
		//for (int i = 0; i < coords.size(); i++)
		//	_root->removeChild(placenodes[i]);
		_root->removeChild(lod);
	}
}
