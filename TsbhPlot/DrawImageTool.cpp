#include "DrawImageTool.h"
#include <osg/ImageStream>

using namespace osgEarth;
DrawImageTool::DrawImageTool(osgEarth::MapNode * mapNode, osg::Group * drawGroup)
	:DrawTool(mapNode, drawGroup)
	, m_ImageNode(0)
	, m_mapNode(mapNode)
{
	m_imageStyle.getOrCreate<IconSymbol>()->declutter() = true;
	m_imageStyle.getOrCreate<IconSymbol>()->alignment() = IconSymbol::ALIGN_CENTER_BOTTOM;
	m_imageStyle.getOrCreate<IconSymbol>()->scale() = 1.0;
	m_imageStyle.getOrCreate<TextSymbol>()->halo() = Color("#5f5f5f");
	m_imageStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
	m_imageStyle.getOrCreate<TextSymbol>()->size() = 16.0;
	m_imageStyle.getOrCreate<osgEarth::TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";//指定中文字体路径
	m_imageStyle.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;

	m_imageStyle.getOrCreate<osgEarth::RenderSymbol>()->lighting() = false;
	m_imageStyle.getOrCreate<osgEarth::RenderSymbol>()->depthTest() = false;
	AltitudeSymbol* alt = m_imageStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

}

DrawImageTool::~DrawImageTool()
{
}

void DrawImageTool::beginDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;
	_centerPoint = lla;

	if (m_ImageNode == 0) {

#if 1
		//原始的绘制图片的代码
		osg::ref_ptr<osg::Image> img = 0L;
		if (m_Iamgepath != "")
		{
			img = osgDB::readImageFile(m_Iamgepath);
		}
		m_ImageNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), _centerPoint), "", m_imageStyle, img);

		m_ImageNode->setDynamic(false);

		osgEarth::Registry::objectIndex()->tagNode(m_ImageNode, m_ImageNode);

		_drawGroup->addChild(m_ImageNode);
#endif

#if 0
		osg::ref_ptr<osgDB::ReaderWriter> rwter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
		osgDB::ReaderWriter::ReadResult rr = rwter->readImage("C:/Users/vmusr/Desktop/hsh.mp4");
		osg::Image* image = rr.getImage();
		osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(image);
		if (imageStream) {
			imageStream->setLoopingMode(osg::ImageStream::LOOPING);
			imageStream->play();
		}

		Style style;
		style.getOrCreate<IconSymbol>()->scale() = 1;
		m_ImageNode = new osgEarth::PlaceNode(GeoPoint(m_mapNode->getMapSRS(), _centerPoint), "",
			style,
			image,
			true,
			osg::Vec3f(0.25, 0.45, 0.1));
		_mapNode->addChild(m_ImageNode);
#endif
	
#if 0
		//视频
		osgDB::Registry::instance()->addFileExtensionAlias("mp4", "ffmpeg");
		osg::ref_ptr<osg::Image> img = 0L;
		
		if (m_Iamgepath != "")
		{
			img = osgDB::readImageFile("C:/Users/vmusr/Desktop/hb.mp4");

			osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(img.get());
			if (imageStream)
			{
				imageStream->play();
				//imageStream->setLoopingMode(osg::ImageStream::LOOPING);
				imageStream->setLoopingMode(osg::ImageStream::NO_LOOPING);
			}
		}

		//图片
		osg::ref_ptr<osg::Image> img = 0L;
		if (m_Iamgepath != "")
		{
			img = osgDB::readImageFile("C:/Users/vmusr/Desktop/touming.png");
		}

		m_ImageNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), _centerPoint), "", m_imageStyle);
		m_ImageNode->setDynamic(false);

		osg::StateSet* stateset = m_ImageNode->getOrCreateStateSet();
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		stateset->setRenderBinDetails(11, "RenderBin");

		m_ImageNode->setIconImage(img);

		osgEarth::Registry::objectIndex()->tagNode(m_ImageNode, m_ImageNode);

		_drawGroup->addChild(m_ImageNode);
#endif
	}
}

void DrawImageTool::moveDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;
	if (m_ImageNode == NULL)
		return;


	m_ImageNode->setPosition(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), lla));
}

void DrawImageTool::endDraw(const osg::Vec3d & lla)
{

	m_temPoint.push_back(lla);
	if (m_ImageNode) {

		m_Property = new ToolProperty;
		m_Property->setNodeStyle(m_imageStyle);
		m_Property->setToolType(IMAGE);

		m_Property->setImagePath(m_Iamgepath);

		m_geoeditr = new GeoPositionNodeEditor(m_ImageNode);
	
		m_geoeditr->setNodeMask(false);

		m_Property->setCenterPoint(_centerPoint);

		m_Property->setDragNode(m_geoeditr);

		m_ImageNode->setUserData(m_Property);

		_drawGroup->addChild(m_geoeditr);
	}
}

void DrawImageTool::resetDraw()
{
	m_temPoint.clear();
	m_ImageNode = NULL;
	_centerPoint = osg::Vec3d();
}

osg::ref_ptr<osg::Node> DrawImageTool::draw(const osg::Vec3d & lla)
{
	if (m_ImageNode == 0) {

		osg::ref_ptr<osg::Image> img = 0L;
		if (m_Iamgepath != "")
		{
			img = osgDB::readImageFile(m_Iamgepath);
		}
		m_ImageNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), lla), "", m_imageStyle, img);

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(m_imageStyle);

		m_Property->setImagePath(m_Iamgepath);

		m_Property->setToolType(IMAGE);

		m_geoeditr = new GeoPositionNodeEditor(m_ImageNode);

		m_geoeditr->setNodeMask(false);

		m_Property->setCenterPoint(lla);

		m_Property->setDragNode(m_geoeditr);

		m_ImageNode->setUserData(m_Property);

		osgEarth::Registry::objectIndex()->tagNode(m_ImageNode, m_ImageNode);

		_drawGroup->addChild(m_geoeditr);

		_drawGroup->addChild(m_ImageNode);

		return m_ImageNode;
	}
	return NULL;
}

void DrawImageTool::setStyle(osgEarth::Style style)
{
	m_imageStyle = style;
}

std::vector<osg::Vec3d> DrawImageTool::getPoints()
{
	return m_temPoint;
}

void DrawImageTool::setImagePath(std::string image)
{
	m_Iamgepath = image;
}

NodeInfo DrawImageTool::getNodeinfos()
{
	NodeInfo info;
	if (m_ImageNode)
	{
		info.node = m_ImageNode;
		info.anneditr = m_geoeditr;
	}
	return info;
}

void DrawImageTool::remove()
{
	if (m_ImageNode)
	{
		_drawGroup->removeChild(m_ImageNode);
		m_ImageNode = 0;
		m_temPoint.clear();
	}
}
