#include "shpPointModelLayer.h"
#include <osgEarth/OGRFeatureSource>
#include "transcoder.h"

using namespace osgEarth;
shpPointModelLayer::shpPointModelLayer() :zeVectorLayer()
{

}
shpPointModelLayer::shpPointModelLayer(std::string name, std::string filePath) : zeVectorLayer(name)
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
	while (cursor.valid() && cursor->hasMore())
	{
		osg::ref_ptr< Feature > feature = cursor->nextFeature();
		osgEarth::Point* point = (osgEarth::Point*)feature->getGeometry();
		osg::Vec3d coord = feature->getGeometry()->at(0);
		if (coord.z() < 0)
			coord.z() = 0;
		osgEarth::AttributeTable table = feature->getAttrs();
		AttributeTable::const_iterator a = table.find(textField);
		std::string text = "";
		if (a != table.end())
		{
			text=a->second.getString();
		}
		if (text == "")
		{
			a = table.find("Name");
			if (a != table.end())
			{
				text = a->second.getString();
			}
		}
		if (text == "")
		{
			a = table.find("NAME");
			if (a != table.end())
			{
				text = a->second.getString();
			}
		}
		//if(feature->getAttrs())
		//feature->getAttrs().find("name")->second();
		coords.push_back(coord);
		texts.push_back(text);
	}
	lod = new osg::LOD;

}
shpPointModelLayer::~shpPointModelLayer()
{

}

void shpPointModelLayer::addToScene(osgEarth::MapNode* root)
{
	const SpatialReference* geoSRS = root->getMapSRS()->getGeographicSRS();
	_root = root;
	if(lod)
	_root->addChild(lod);
	//构建placenode集合
	osgEarth::Style style;
	style.getOrCreate<IconSymbol>()->declutter() = iconDeclutter;
	style.getOrCreate<IconSymbol>()->alignment() = IconAlignment;
	style.getOrCreate<IconSymbol>()->scale() = this->iconScale;
	style.getOrCreate<TextSymbol>()->halo()->color() = textFill;
	style.getOrCreate<IconSymbol>()->url()->setLiteral(this->imagePath);
	style.getOrCreate<TextSymbol>()->alignment() = textAlignment;
	style.getOrCreate<TextSymbol>()->size() = textSize;
	std::string::size_type pos1 = textFont.find("/");
	std::string::size_type pos2 = textFont.find("\\");
	if (pos1 != std::string::npos&&pos1 != std::string::npos)
	{
		style.getOrCreate<osgEarth::TextSymbol>()->font() = textFont;//指定中文字体路径
		
	}
	else
	{
		style.getOrCreate<osgEarth::TextSymbol>()->font() = "C:/Windows/Fonts/" + textFont;
	}
	//style.getOrCreate<osgEarth::TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";//指定中文字体路径
	style.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;

	style.getOrCreate<osgEarth::RenderSymbol>()->lighting() = lighting;
	style.getOrCreate<osgEarth::RenderSymbol>()->depthTest() = depthTest;
	AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	for (int i = 0; i < coords.size(); i++)
	{
		std::string ss= texts[i];
		if(textEncoding!="utf-8")
		{ 
		
		Transcoder::gb2312ToUtf8(texts[i], ss);
		}
		osgEarth::PlaceNode* placenode = new PlaceNode(GeoPoint(geoSRS, coords[i].x(), coords[i].y(), coords[i].z()),ss, style);
		lod->addChild(placenode, this->minrange, this->maxrange);
		placenodes.push_back(placenode);
	}
}

void shpPointModelLayer::remove()
{
	if (_root)
	{
		//for (int i = 0; i < coords.size(); i++)
		//	_root->removeChild(placenodes[i]);
		_root->removeChild(lod);
	}
}
