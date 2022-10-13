#include "DrawGeoPoint.h"

DrawGeoPoint::DrawGeoPoint(osgEarth::MapNode * mapNode, osg::Group * drawGroup)
	:DrawTool(mapNode, drawGroup)
	, m_GeoPositionNode(0)
	, m_mapNode(mapNode)
{

	m_GeoPointStyle.getOrCreate<PolygonSymbol>()->fill()->color()
		= osgEarth::Color(osgEarth::Color::Yellow, 0.25);

	LineSymbol* ls = m_GeoPointStyle.getOrCreate<LineSymbol>();
	ls->stroke()->color() = osgEarth::Color::White;
	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;

	//    RenderSymbol* render = _circleStyle.getOrCreate<RenderSymbol>();
	//    render->depthOffset()->enabled() = true;
	//    render->depthOffset()->automatic() = true;

	AltitudeSymbol* alt = m_GeoPointStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;
}

DrawGeoPoint::~DrawGeoPoint()
{
}

void DrawGeoPoint::beginDraw(const osg::Vec3d & lla)
{
	if (_centerPoint == osg::Vec3d() && lla == osg::Vec3d()) {
		return;
	}
	else if (_centerPoint == osg::Vec3d()) {
		_centerPoint = lla;
	}

	if (_circleNode == 0) {
		_circleNode = new CircleNode();
		_circleNode->set(
			GeoPoint::GeoPoint(m_mapNode->getMapSRS(), _centerPoint),
			Distance(5, Units::KILOMETERS), m_GeoPointStyle,
			Angle(0.0, Units::DEGREES))
			;
		_circleEdit = new GeoPositionNodeEditor(_circleNode);

		_circleEdit->setNodeMask(false);

		osgEarth::Registry::objectIndex()->tagNode(_circleNode, _circleNode);

		m_posNode = renderCircleNode(new GeoPoint(m_mapNode->getMapSRS(), _centerPoint.x(), _centerPoint.y(), _centerPoint.z()));
		_drawGroup->addChild(_circleNode);
		_drawGroup->addChild(_circleEdit);
	}


#ifdef SHOW_CIRCLE_RADIUS
	if (!_radiusNode.valid()) {
		_radiusNode = new PlaceNode(getMapNode(), midPoint, NULL, osgEarth::Stringify() << "r=" << radius);
		drawCommand(_radiusNode);
	}
	_radiusNode->setPosition(midPoint);
	_radiusNode->setText(osgEarth::Stringify() << "r=" << osgEarth::Distance(radius).as(Units::KILOMETERS) << "km");
#endif

}

void DrawGeoPoint::moveDraw(const osg::Vec3d & lla)
{
}

void DrawGeoPoint::endDraw(const osg::Vec3d & lla)
{
	if (_circleNode) {
		m_Property = new ToolProperty;
		m_Property->setNodeStyle(m_GeoPointStyle);
		m_Property->setToolType(GEOPOSITION);
		m_Property->setCenterPoint(_centerPoint);
		m_Property->setDragNode(_circleEdit);
		m_Property->setPosNode(m_posNode);
		_circleNode->setUserData(m_Property);
	}
}

void DrawGeoPoint::resetDraw()
{
	_centerPoint = osg::Vec3d();
	_circleNode = NULL;
}

osg::ref_ptr<osg::Node> DrawGeoPoint::draw(const osg::Vec3d & lla)
{
	if (lla.isNaN())
		return NULL;
	_circleNode = new CircleNode();
	_circleNode->set(GeoPoint::GeoPoint(m_mapNode->getMapSRS(), lla),
		Distance(5, Units::KILOMETERS), m_GeoPointStyle,
		Angle(0.0, Units::DEGREES));

	m_Property = new ToolProperty;

	m_Property->setNodeStyle(m_GeoPointStyle);

	m_Property->setToolType(GEOPOSITION);

	m_Property->setCenterPoint(lla);

	_circleEdit = new GeoPositionNodeEditor(_circleNode);

	_circleEdit->setNodeMask(false);

	m_Property->setDragNode(_circleEdit);

	m_posNode = renderCircleNode(new GeoPoint(m_mapNode->getMapSRS(), lla.x(), lla.y(), lla.z()));

	m_Property->setPosNode(m_posNode);

	_circleNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(_circleNode, _circleNode);

	_drawGroup->addChild(_circleNode);

	_drawGroup->addChild(_circleEdit);

	return _circleNode;
}

void DrawGeoPoint::setStyle(osgEarth::Style style)
{
	m_GeoPointStyle = style;
}

osg::ref_ptr<osg::MatrixTransform> DrawGeoPoint::renderCircleNode(GeoPoint * point)
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

	_drawGroup->addChild(_transfer);

	return _transfer;
}

std::vector<osg::Vec3d> DrawGeoPoint::getPoints()
{
	return std::vector<osg::Vec3d>();
}

NodeInfo DrawGeoPoint::getNodeinfos()
{
	NodeInfo info;
	if (_circleNode)
	{
		info.node = _circleNode;
		info.anneditr = _circleEdit;
	}

	return info;
}

void DrawGeoPoint::remove()
{
	if (_circleNode)
	{
		_drawGroup->removeChild(_circleNode);
		_drawGroup->removeChild(m_posNode);
		_circleNode = nullptr;
		m_posNode = nullptr;
		_centerPoint = osg::Vec3d();
	}
}
