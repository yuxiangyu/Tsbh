#include "DrawCircleTool.h"
#include <osgEarth/GeoMath>
#include <osgEarth/Units>

#define LC "[DrawCircleTool] "

using namespace osgEarth;


DrawCircleTool::DrawCircleTool(MapNode* mapNode, osg::Group* drawGroup)
    : DrawTool(mapNode, drawGroup)
	, m_mapNode(mapNode)
{
    //getDrawGroup()->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    _circleStyle.getOrCreate<PolygonSymbol>()->fill()->color()=osgEarth::Color(osgEarth::Color::Yellow, 0.25);

    LineSymbol* ls = _circleStyle.getOrCreate<LineSymbol>();
    ls->stroke()->color() = osgEarth::Color::White;
    ls->stroke()->width() = 2.0f;
    ls->stroke()->widthUnits() = Units::PIXELS;

    //    RenderSymbol* render = _circleStyle.getOrCreate<RenderSymbol>();
    //    render->depthOffset()->enabled() = true;
    //    render->depthOffset()->automatic() = true;

    AltitudeSymbol* alt = _circleStyle.getOrCreate<AltitudeSymbol>();
    alt->verticalOffset() = 0.1;
    alt->clamping() = alt->CLAMP_TO_TERRAIN;
    alt->technique() = alt->TECHNIQUE_DRAPE;

}

void DrawCircleTool::beginDraw(const osg::Vec3d& lla)
{
    if (_centerPoint == osg::Vec3d() && lla == osg::Vec3d()) {
        return;
    } else if (_centerPoint == osg::Vec3d()) {
        _centerPoint = lla;
    }

    if (_circleNode == 0) {
		_circleNode = new CircleNode();
		_circleNode->set(
			GeoPoint::GeoPoint(m_mapNode->getMapSRS(), _centerPoint),
			Distance(0, Units::METERS), _circleStyle,
			Angle(0.0, Units::DEGREES))
            ;
        _circleEdit = new CircleNodeEditor(_circleNode);
    
		_circleEdit->setNodeMask(false);

		osgEarth::Registry::objectIndex()->tagNode(_circleNode, _circleNode);

		_drawGroup->addChild(_circleNode);
		_drawGroup->addChild(_circleEdit);
    }

    double radius = GeoMath::distance(_centerPoint, lla, m_mapNode->getMapSRS());

    osg::Vec3d midVec((lla.x() + _centerPoint.x()) / 2, (lla.y() + _centerPoint.y()) /2 , (lla.z() +_centerPoint.z()) / 2);
    osgEarth::GeoPoint midPoint(m_mapNode->getMapSRS(), midVec);

#ifdef SHOW_CIRCLE_RADIUS
    if (!_radiusNode.valid()) {
        _radiusNode = new PlaceNode(getMapNode(), midPoint, NULL, osgEarth::Stringify()<<"r="<< radius);
        drawCommand(_radiusNode);
    }
    _radiusNode->setPosition(midPoint);
    _radiusNode->setText(osgEarth::Stringify()<<"r="<< osgEarth::Distance(radius).as(Units::KILOMETERS) << "km");
#endif

    _circleNode->setRadius(Distance(radius, Units::METERS));

	//_circleNode->setArcStart(Angle(0.0, Units::DEGREES));

	//_circleNode->setArcEnd(Angle(120.0, Units::DEGREES));

	//_circleNode->setPie(true);

}

void DrawCircleTool::moveDraw(const osg::Vec3d& lla)
{
    if (_circleNode != 0 && _centerPoint != osg::Vec3d()) {
        osg::Vec3d midVec((lla.x() + _centerPoint.x()) / 2, (lla.y() + _centerPoint.y()) /2 , (lla.z() +_centerPoint.z()) / 2);
        osgEarth::GeoPoint midPoint(m_mapNode->getMapSRS(), midVec);
         double radius = GeoMath::distance(_centerPoint, lla, m_mapNode->getMapSRS());
        _circleNode->setRadius(Distance(radius, Units::METERS));
#ifdef SHOW_CIRCLE_RADIUS
        _radiusNode->setPosition(midPoint);
        _radiusNode->setText(osgEarth::Stringify()<<"r="<< osgEarth::Distance(radius).as(Units::KILOMETERS) << "km");
#endif
    }
}

void DrawCircleTool::endDraw(const osg::Vec3d& lla)
{
	_temPoint.push_back(_centerPoint);
	_temPoint.push_back(lla);
	if (_circleNode) {
		m_Property = new ToolProperty;
		m_Property->setNodeStyle(_circleStyle);
		m_Property->setToolType(CIRCLE);
		m_Property->setDisPoint(lla);
		m_Property->setCenterPoint(_centerPoint);
		m_Property->setDragNode(_circleEdit);
		_circleNode->setUserData(m_Property);
	}
}

void DrawCircleTool::resetDraw()
{
    _centerPoint = osg::Vec3d();
    _circleNode = NULL;
	_temPoint.clear();
#ifdef SHOW_CIRCLE_RADIUS
    _radiusNode = NULL;
#endif
}

osg::ref_ptr<osg::Node>  DrawCircleTool::draw(osg::Vec3d cneterPoint, osg::Vec3d distancepoint)
{

	if (cneterPoint.isNaN()|| distancepoint.isNaN())
		 return NULL;
	_circleNode = new CircleNode();
	_circleNode->set(GeoPoint::GeoPoint(m_mapNode->getMapSRS(), cneterPoint),
		Distance(0, Units::METERS), _circleStyle,
		Angle(0.0, Units::DEGREES));
	//	_circleEdit = new CircleNodeEditor(_circleNode);
	 double radius = GeoMath::distance(cneterPoint, distancepoint, m_mapNode->getMapSRS());

	osg::Vec3d midVec((distancepoint.x() + _centerPoint.x()) / 2, (distancepoint.y() + _centerPoint.y()) / 2, (distancepoint.z() + _centerPoint.z()) / 2);
	osgEarth::GeoPoint midPoint(m_mapNode->getMapSRS(), midVec);

	_circleNode->setRadius(Distance(radius, Units::METERS));

	m_Property = new ToolProperty;

	m_Property->setNodeStyle(_circleStyle);

	m_Property->setToolType(CIRCLE);

	m_Property->setDisPoint(distancepoint);

	m_Property->setCenterPoint(cneterPoint);
    
	_circleEdit = new CircleNodeEditor(_circleNode);

	_circleEdit->setNodeMask(false);

	m_Property->setDragNode(_circleEdit);

	_circleNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(_circleNode, _circleNode);

	_drawGroup->addChild(_circleNode);

	_drawGroup->addChild(_circleEdit);

	return _circleNode;
}

void DrawCircleTool::setStyle(osgEarth::Style style)
{
	_circleStyle = style;
}

std::vector<osg::Vec3d> DrawCircleTool::getPoints()
{
	return _temPoint;
}

void DrawCircleTool::remove()
{
	if (_circleNode)
	{
		_drawGroup->removeChild(_circleNode);
		_circleNode = NULL;
		_centerPoint = osg::Vec3d();
	}
}

NodeInfo DrawCircleTool::getNodeinfos()
{
	NodeInfo info;
	if (_circleNode)
	{
		info.node = _circleNode;
		info.anneditr = _circleEdit;
	}

	return info;
}
