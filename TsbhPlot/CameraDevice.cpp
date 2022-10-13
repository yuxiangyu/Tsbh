#include "CameraDevice.h"
#include "transcoder.h"
#include <osgEarth/Registry>
#include <osgEarth/ObjectIndex>
#include "AnnotationEditing.h"
#include <qdebug.h>



CameraDevice::CameraDevice(osg::Group* group, osgEarth::MapNode* node)
{
	mp_group = group;
	this->mapNode = node;
	pm.getOrCreate<IconSymbol>()->declutter() = true;
	//pm.getOrCreate<IconSymbol>()->alignment() = IconSymbol::ALIGN_CENTER_CENTER;
	pm.getOrCreate<IconSymbol>()->alignment() = IconSymbol::ALIGN_CENTER_BOTTOM;
	pm.getOrCreate<IconSymbol>()->scale() = 1.2;			//图标大小
	pm.getOrCreate<TextSymbol>()->halo() = Color("#5f5f5f");
	pm.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
	pm.getOrCreate<TextSymbol>()->size() = 16.0;
	pm.getOrCreate<osgEarth::TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";//指定中文字体路径
	pm.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;

	//画圆
	mp_group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	m_circleStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(0, 0, 1, 0.5);// osgEarth::Color::Yellow;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	m_circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	LineSymbol* ls = m_circleStyle.getOrCreate<LineSymbol>();
	ls->stroke()->color() = Color(1 / 255.0, 255 / 255.0, 238 / 255.0, 100 / 255.0);
	ls->stroke()->width() = 1.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 5;
	//this->values = values;
}


CameraDevice::~CameraDevice()
{
}

//模型构建
void CameraDevice::buildModel()
{
	if (m_modeldatas.isEmpty())
		return;
	for each (auto var in m_modeldatas)
	{		
		//std::string iconPath = getIconPath(var.type);
		std::string iconPath = var.iconPath;
		pm.getOrCreate<IconSymbol>()->url()->setLiteral(iconPath);
		std::string _strWideName = var.name.toStdString();
		//Transcoder::gb2312ToUtf8(Transcoder::qstr2str(var.name), _strWideName);

		osgEarth::GeoPoint _centerPoint(mapNode->getMapSRS()->getGeographicSRS(), var.positionx, var.positiony, 0.0, ALTMODE_ABSOLUTE);

		osg::ref_ptr<osgEarth::PlaceNode> mLabel = new osgEarth::PlaceNode(_centerPoint, _strWideName, pm);
		mLabel->setDynamic(true);
		mLabel->setName(var.name.toStdString());

		//osgEarth::Registry::objectIndex()->tagNode(mLabel, mLabel);
		//
		mp_group->addChild(mLabel);

		//if (var["TYPE"].toInt() == 6)
		//{
		//
		//	osg::ref_ptr<osgEarth::CircleNode> circle = new osgEarth::CircleNode();

		//	circle->set(
		//		_centerPoint,
		//		Distance(0, Units::KILOMETERS),
		//		m_circleStyle,
		//		Angle(var["ANGLESTART"].toString().toFloat(), Units::DEGREES),
		//		Angle(var["ALGLEEND"].toString().toFloat(), Units::DEGREES),
		//		true);
		//	circle->setNumSegments(2);

		//	//double radius = GeoMath::distance(osg::Vec3d(_centerPoint.x(), _centerPoint.y(), _centerPoint.z()), osg::Vec3d(_centerPoint.x() + 0.5, _centerPoint.y() + 0.5, _centerPoint.z()), mapNode->getMapSRS()->getGeographicSRS());
		//	circle->setRadius(Distance(var["SCOPE"].toString().toFloat(), Units::METERS));

		//	lod1->addChild(circle,0,500000);

		//	QMap<QString, osg::ref_ptr<osgEarth::CircleNode>>criclevalue;

		//	criclevalue[var["ID"].toString()] = circle;

		//	m_circles.push_back(criclevalue);
		//	
		//}
		//QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>Labelvalue;
		m_Labels[var.id] = mLabel;
	}	
}

void CameraDevice::setPosition(QString id,GeoPoint& gp)
{
	if (m_Labels.empty())
	{
		return;
	}
	else {
		QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>::iterator iter = m_Labels.find(id);
		if (iter != m_Labels.end())
		{
			//qDebug() << gp.x() << gp.y();
			osgEarth::GeoPoint point(mapNode->getMapSRS()->getGeographicSRS(), gp.x(), gp.y(), 0.0, ALTMODE_ABSOLUTE);
			//mLabel->setPosition(point);
			iter.value()->setPosition(point);
		}
	}
}

void CameraDevice::setModelDatas(QList<S2DModelInfo> values)
{
	if (values.isEmpty())
		return;
	m_modeldatas = values;
}

void CameraDevice::set2DModelMask(QString id, bool visible)
{
	if (m_Labels.empty())
	{
		return;
	}
	else {
		QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>::iterator iter = m_Labels.find(id);
		if (iter != m_Labels.end())
		{
			iter.value()->setNodeMask(visible);
		}
	}
}

void CameraDevice::setModelsVisible(bool visble)
{
	if (m_Labels.isEmpty())
		return;

	for (QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>::const_iterator iter = m_Labels.begin()
			; iter != m_Labels.end(); iter++)
		{
			iter.value()->setNodeMask(visble);
		}
	
}

void CameraDevice::removeOne(QString id)
{

	if (m_Labels.empty())
	{
		return;
	}
	else {
		QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>::iterator iter = m_Labels.find(id);
		if (iter != m_Labels.end())
		{
			mp_group->removeChild(iter.value());
		}
	}
}

void CameraDevice::removeAll()
{
	if (m_Labels.isEmpty())
		return;

	for (QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>::const_iterator iter = m_Labels.begin()
		; iter != m_Labels.end(); iter++)
	{
		mp_group->removeChild(*iter);
	}
}

std::string CameraDevice::getIconPath(int iocnType)
{
	
	/*
	* 20220215 - gkk 注释，该部分引用了RtssData中的类和API，当前RtssData模块为空的，暂不引用
	QList<CurrentModel> modelicons = TszsData::getInstacne()->GetCurrentModel();
	for each (auto var in modelicons)
	{
		if (var.ModelType == iocnType)
		{
			return var.ModelPath.toStdString();
		}	
	}
	*/
	return "";
}

osg::ref_ptr<osg::Node> CameraDevice::getNode(QString nodeID)
{
	if (m_Labels.contains(nodeID))
	{
		return m_Labels[nodeID];
	}
	return nullptr;
}
