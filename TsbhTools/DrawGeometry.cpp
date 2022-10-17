
#include "DrawGeometry.h"
#include <osgEarth/MapNode>
#include <osgEarth/GeoMath>
#include "transcoder.h"
#include<osgDB\ReadFile>
#include <QApplication>
#include <osg/LOD>
DrawGeometry::DrawGeometry(osg::LOD* group, osgEarth::MapNode* node)
			:m_startAngle(0)
			,m_endAngle(360)
{
	this->m_mapNode = node;
	this->m_root = group;
	m_centerPoint = osg::Vec3d();
	m_circleRadius = 0;
	for(int i=0;i<MAXPOLYGONCOUNT;i++)
	  m_polygon[i] = new osgEarth::Polygon();

	//文字样式
	m_pin.getOrCreate<osgEarth::TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";//指定中文字体路径
	m_pin.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	m_pin.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
	m_pin.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	m_pin.getOrCreate<TextSymbol>()->size() = 16.0;
	m_pin.getOrCreate<IconSymbol>()->declutter() = true;

	//图片类型映射
	QString appstr = QApplication::applicationDirPath();
	m_mapTypeImg.insert(std::pair<int, std::string>(0, appstr.toStdString() + "/ResImg/兵要/指挥所.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(1, appstr.toStdString() + "/ResImg/兵要/出入口.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(2, appstr.toStdString() + "/ResImg/兵要/通讯部.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(3, appstr.toStdString() + "/ResImg/兵要/火车站.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(4, appstr.toStdString() + "/ResImg/兵要/车库.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(5, appstr.toStdString() + "/ResImg/兵要/训练场.png"));
	m_mapTypeImg.insert(std::pair<int, std::string>(6, appstr.toStdString() + "/ResImg/兵要/弹药库.png"));
}

DrawGeometry::~DrawGeometry()
{
}

void DrawGeometry::initCircle()
{
	m_root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	//osg::ref_ptr<CircleNode> pCircleNode = NULL;

	m_circleStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(0, 0, 1, 0.5);// osgEarth::Color::Yellow;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	LineSymbol* ls = m_circleStyle.getOrCreate<LineSymbol>();
	ls->stroke()->color() = Color::Red;
	ls->stroke()->width() = 1.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 30;

	osg::ref_ptr<CircleNode> pCircleNode = new osgEarth::CircleNode();
	pCircleNode->set(
		GeoPoint(m_mapNode->getMapSRS(), m_centerPoint, ALTMODE_ABSOLUTE),
		Distance(0, Units::METERS),
		m_circleStyle,
		Angle(m_startAngle, Units::DEGREES),
		Angle(m_endAngle, Units::DEGREES),
		true);

	
	std::string utfStr;
	osg::ref_ptr<osg::Image> img;
	if(!m_sName.isEmpty())
	{
		std::string stdStr = m_sName.toLocal8Bit();
		utfStr = m_mapTypeImg[m_nType];
		img = osgDB::readImageFile(utfStr);
		utfStr.clear();
		Transcoder::gb2312ToUtf8(stdStr, utfStr);
	}
	osg::ref_ptr<osgEarth::PlaceNode> nameNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), m_centerPoint), utfStr, m_pin,img);
	//osg::LOD* lod = dynamic_cast<osg::LOD*>(m_root);
	m_root->addChild(pCircleNode,0,1e6);
	m_root->addChild(nameNode,0,1e6);
	m_sSubtasks.type = 1;
	addGeometry(m_sSubtasks, pCircleNode);	
	double radius = GeoMath::distance(osg::Vec3d(m_centerPoint.x(), m_centerPoint.y(), m_centerPoint.z()), osg::Vec3d(m_centerPoint.x()+ m_circleRadius, m_centerPoint.y()+ m_circleRadius, m_centerPoint.z()), m_mapNode->getMapSRS()->getGeographicSRS());
	pCircleNode->setRadius(Distance(m_circleRadius, Units::METERS));
}

void DrawGeometry::setCircleCenterPoint(osg::Vec3d centerPoint, double radius)
{
	m_centerPoint = centerPoint;
	m_circleRadius = radius;
}
void DrawGeometry::setNameAndIcon(QString sName, int Type)
{
	m_sName = sName;
	m_nType = Type;
}

void DrawGeometry::setCircleAngle(double startangle,double endangle)
{
	m_startAngle = startangle;
	m_endAngle = endangle;
}
void DrawGeometry::drawPolygon(int index,osg::Vec3d vec)
{
	osg::ref_ptr<osgEarth::Feature> ptrFeature = new Feature(m_polygon[index], m_mapNode->getMapSRS()->getGeographicSRS());
	osg::ref_ptr<osgEarth::FeatureNode> ptrFeatureNode = new FeatureNode(ptrFeature, setGeometryStyle());
	ptrFeatureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	ptrFeatureNode->dirty();

	m_root->addChild(ptrFeatureNode,0,1e6);

	

	if (!m_sName.isEmpty())
	{
		std::string utfStr;
		osg::ref_ptr<osg::Image> img;
		std::string stdStr = m_sName.toLocal8Bit();
		utfStr = m_mapTypeImg[m_nType];
		img = osgDB::readImageFile(utfStr);
		utfStr.clear();
		Transcoder::gb2312ToUtf8(stdStr, utfStr);
		osg::ref_ptr<osgEarth::PlaceNode> nameNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), vec), utfStr, m_pin, img);
		m_root->addChild(nameNode,0,1e6);
	}
	

	m_sSubtasks.type = 2;
	addGeometry(m_sSubtasks,ptrFeatureNode);

	ptrFeature->geoInterp() = GEOINTERP_RHUMB_LINE;
	ptrFeatureNode->dirty();
}

Style DrawGeometry::setGeometryStyle()
{
	Style style;
	style.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = Color(osgEarth::Color::Yellow, 0.25);
	AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
	alt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
	alt->verticalOffset() = 0.1;
	alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	RenderSymbol* render = style.getOrCreate<RenderSymbol>();
	render->minAlpha() = 0.15f;
	LineSymbol* ls = style.getOrCreate<LineSymbol>();

	//int aa = (int)10*static_cast<double>(::rand()) / static_cast<double>(RAND_MAX);

	ls->stroke()->color() = Color::Red;

	ls->stroke()->width() = 1.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 30;

	return style;
}

void DrawGeometry::addPolygonPoint(double x,double y,double z,int index)
{
	m_polygon[index]->push_back(osg::Vec3d(x,y,z));
}

void DrawGeometry::clearPolygon(int index)
{
	m_polygon[index]->clear();

}
void DrawGeometry::setSubtasksId(std::string id) 
{ 
	m_sSubtasks.id = id; 
}
void DrawGeometry::addGeometry(SGeometry gtyKey, void *parameter)
{
	m_geometryPtr[gtyKey] = parameter;
}
void DrawGeometry::removeGeometry(SGeometry gtyKey)
{
	std::map<SGeometry, void *>::iterator mapiter;
	mapiter = m_geometryPtr.find(gtyKey);
	if (mapiter != m_geometryPtr.end())
	{
		switch (mapiter->first.type)
		{
		case 1:
			m_root->removeChild((CircleNode*)mapiter->second);
			m_geometryPtr.erase(mapiter);
			break;
		case 2:
			m_root->removeChild((FeatureNode*)mapiter->second);
			m_geometryPtr.erase(mapiter);
			break;
		default:
			break;
		}
	}
}
void DrawGeometry::setGeometryVisible(std::string index, bool state,int type)
{
	SGeometry gtyKey;
	gtyKey.id = index;
	gtyKey.type = type;
	std::map<SGeometry, void *>::iterator mapiter;
	mapiter = m_geometryPtr.find(gtyKey);
	if (mapiter != m_geometryPtr.end())
	{
		switch (mapiter->first.type)
		{
		case 1:
			((CircleNode*)mapiter->second)->setNodeMask(state);
			break;
		case 2:
			((FeatureNode*)mapiter->second)->setNodeMask(state);
			break;
		default:
			break;
		}
	}
}
