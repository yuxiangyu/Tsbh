#include "DrawTextTool.h"

using namespace osgEarth;
#include <QDebug>
DrawTextTool::DrawTextTool(osgEarth::MapNode * mapNode, osg::Group * drawGroup)
: DrawTool(mapNode, drawGroup)
, _feature(0)
, m_labelNode(0)
, _stippleFeature(0)
, m_stippleLabelNode(0)
, m_mapNode(mapNode)
{
	//m_stream.precision(1);
	//m_stream << u8"           ";
	m_TextStyle.getOrCreate<TextSymbol>()->size() = 18.0f;
	m_TextStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	m_TextStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	m_TextStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	m_TextStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	m_TextStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

	AltitudeSymbol* alt = m_TextStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	//AltitudeSymbol* lalt = m_TextStyle.getOrCreate<AltitudeSymbol>();
	//lalt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;				//贴地，线不是直线
	//																	//lalt->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;		//不显示
	//																	//lalt->clamping() = AltitudeSymbol::CLAMP_NONE;					//不贴地
	//																	//lalt->clamping() = AltitudeSymbol::CLAMP_ABSOLUTE;				//不贴地
	//																	//lalt->technique() = AltitudeSymbol::TECHNIQUE_GPU;				//运动时线会抖动
	//lalt->technique() = AltitudeSymbol::TECHNIQUE_MAP;					//运动时线不会抖动
	//																	//lalt->technique() = AltitudeSymbol::TECHNIQUE_SCENE;				//运动时线不会抖动
	//																	//lalt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;				//画出的线会变粗
	//lalt->verticalOffset() = 0.1;

}

void DrawTextTool::beginDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;

	_centerPoint = lla;


	if (m_labelNode == 0) {
		//m_Text = m_stream.str();
		m_labelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), lla), m_Text, m_TextStyle, NULL);


		osgEarth::Registry::objectIndex()->tagNode(m_labelNode, m_labelNode);
		_drawGroup->addChild(m_labelNode);

		//m_labelNode->setUserData(m_Property);
	}
}

void DrawTextTool::moveDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;
	if (m_labelNode == NULL)
		return;


	m_labelNode->setPosition(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), lla));
}

void DrawTextTool::endDraw(const osg::Vec3d & lla)
{
	_temPoint.push_back(lla);

	if (m_labelNode) {

		m_geoeditr = new GeoPositionNodeEditor(m_labelNode);

		m_geoeditr->setNodeMask(false);

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(m_TextStyle);

		m_Property->setToolType(TEXT);

		m_Property->setCenterPoint(lla);

		m_Property->setDragNode(m_geoeditr);

		m_labelNode->setUserData(m_Property);

		_drawGroup->addChild(m_geoeditr);
	}
}

void DrawTextTool::resetDraw()
{

		_temPoint.clear();
		m_labelNode = NULL;
		
		_centerPoint = osg::Vec3d();
}

osg::ref_ptr<osg::Node> DrawTextTool::draw(osg::Vec3d cneterPoint)
{

	/*std::stringstream stream;
	stream.precision(1);
	stream << u8"左键单击确定地点，右键结束";
	m_Text = m_stream.str();*/
	//构造提示label
	m_labelNode = new  osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), cneterPoint), m_Text, m_TextStyle, NULL);
	
	m_Property = new ToolProperty;

	m_Property->setNodeStyle(m_TextStyle);

	m_Property->setToolType(TEXT);

	m_geoeditr = new GeoPositionNodeEditor(m_labelNode);

	m_geoeditr->setNodeMask(false);

	m_Property->setCenterPoint(cneterPoint);

	m_Property->setDragNode(m_geoeditr);

	m_labelNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(m_labelNode, m_labelNode);

	_drawGroup->addChild(m_geoeditr);
	_drawGroup->addChild(m_labelNode);

	return m_labelNode;
}

void DrawTextTool::setStyle(osgEarth::Style style)
{
	m_TextStyle = style;
}

std::vector<osg::Vec3d> DrawTextTool::getPoints()
{
	return _temPoint;
}

void DrawTextTool::remove()
{
	if (m_labelNode)
	{
		_drawGroup->removeChild(m_labelNode);
		m_labelNode = 0;
		_temPoint.clear();
	}
}

NodeInfo DrawTextTool::getNodeinfos()
{
	NodeInfo info;
	if (m_labelNode)
	{
		info.node = m_labelNode;
		info.anneditr = m_geoeditr;
	}
	return info;
}

DrawTextTool::~DrawTextTool()
{
}
