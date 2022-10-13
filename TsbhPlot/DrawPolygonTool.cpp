#include "DrawPolygonTool.h"
#include "FeatureEditing.h"
#include <qdebug.h>
using namespace osgEarth;


DrawPolygonTool::DrawPolygonTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup)
    : DrawTool(mapNode, drawGroup)
    , _featureNode(nullptr)
    , _stippleFeatureNode(nullptr)
    , _polygonEdit(nullptr)
{

    _polygonStyle.getOrCreate<PolygonSymbol>()
        ->fill()
        ->color()
        = Color(1/255.0,255/255.0,238/255.0, 100/255.0);

    LineSymbol* ls = _polygonStyle.getOrCreate<LineSymbol>();
    ls->stroke()->color() = Color(8/255.0,140/255.0,255/255.0,200/255.0);
    ls->stroke()->width() = 2.0f;
    ls->stroke()->widthUnits() = osgEarth::Units::PIXELS;

    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->clamping()
        = AltitudeSymbol::CLAMP_TO_TERRAIN;
    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->technique()
        = AltitudeSymbol::TECHNIQUE_GPU;
    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->verticalOffset()
        = 0.1;

    _stippleLineStyle.getOrCreate<LineSymbol>()
        ->stroke()
        ->color()
        = Color::Red;
    _stippleLineStyle.getOrCreate<LineSymbol>()
        ->stroke()
        ->width()
        = 2.0;
    _stippleLineStyle.getOrCreate<LineSymbol>()
        ->tessellation()
        = 20.0;
    _stippleLineStyle.getOrCreate<AltitudeSymbol>()
        ->clamping()
        = AltitudeSymbol::CLAMP_TO_TERRAIN;
    _stippleLineStyle.getOrCreate<AltitudeSymbol>()
        ->technique()
        = AltitudeSymbol::TECHNIQUE_GPU;
    _stippleLineStyle.getOrCreate<AltitudeSymbol>()
        ->verticalOffset()
        = 0.1;
    _stippleLineStyle.getOrCreate<LineSymbol>()
        ->stroke()
        ->stipple()
        = 255;


}

void DrawPolygonTool::beginDraw(const osg::Vec3d& lla)
{
    _vecPoints.push_back(lla);
    if (_vecPoints.size() <= 2) {
        return;
    }

    //if (_polygonEdit != nullptr) {
    //    _polygonEdit->removeChildren(0, _polygonEdit->getNumChildren());
    //    _polygonEdit = nullptr;
    //}
    if (_featureNode == nullptr) {
        Feature* feature = new Feature(
            new Polygon,
            getMapNode()->getMapSRS());

        _featureNode = new FeatureNode(
            feature, _polygonStyle);
		_featureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

		osgEarth::Registry::objectIndex()->tagNode(_featureNode, _featureNode);

		_drawGroup->addChild(_featureNode);
    }

    Geometry* geom = _featureNode->getFeature()->getGeometry();
    geom->clear();
    _featureNode->setStyle(_polygonStyle);
    for (auto& n : _vecPoints) {
        geom->push_back(n);
    }

	_featureNode->dirty();

    if (_stippleFeatureNode != nullptr) {
        _stippleFeatureNode->getFeature()->getGeometry()->clear();
    }
	
}

void DrawPolygonTool::moveDraw(const osg::Vec3d& lla)
{
    if (_vecPoints.size() < 2) {
        return;
    }
    if (_stippleFeatureNode == nullptr) {
        Feature* feature = new Feature(
            new LineString,
            getMapNode()->getMapSRS());
        _stippleFeatureNode = new FeatureNode(
           feature, _stippleLineStyle);
		_stippleFeatureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		_drawGroup->addChild(_stippleFeatureNode);
    }

    Geometry* geom = _stippleFeatureNode->getFeature()->getGeometry();
    geom->clear();
    _stippleFeatureNode->setStyle(_stippleLineStyle);
    geom->push_back(_vecPoints[0]);
    geom->push_back(lla);
    geom->push_back(_vecPoints[_vecPoints.size() - 1]);

    _stippleFeatureNode->dirty();
}

void DrawPolygonTool::endDraw(const osg::Vec3d& lla)
{
	if (_featureNode) {

	m_Property = new ToolProperty;

	m_Property->setNodeStyle(_polygonStyle);

	m_Property->setToolType(POLYGON);

	_polygonEdit = new FeatureEditor(_featureNode);
	_polygonEdit->setNodeMask(false);

	m_Property->setPoints(_vecPoints);
	m_Property->setDragNode(_polygonEdit);

	_drawGroup->addChild(_polygonEdit);

	_featureNode->setUserData(m_Property);
  }
	_temPoint.clear();
	_temPoint = _vecPoints;
	//if (_stippleFeatureNode)
	//{
	//	_drawGroup->removeChild(_stippleFeatureNode);
	//	_stippleFeatureNode = 0;
	//	setActive(false);
	//}
}

void DrawPolygonTool::resetDraw()
{
    _vecPoints.clear();
    if (_stippleFeatureNode != nullptr) {
        _stippleFeatureNode->getFeature()->getGeometry()->clear();
		_drawGroup->removeChild(_stippleFeatureNode);
		_stippleFeatureNode = NULL;
    }

	_featureNode = nullptr;
}

osg::ref_ptr<osg::Node> DrawPolygonTool::draw(std::vector<osg::Vec3d> points)
{
	if (points.size() <= 2) {
		return NULL;
	}
		Feature* feature = new Feature(
			new Polygon,
			getMapNode()->getMapSRS());

		_featureNode = new FeatureNode(
			feature, _polygonStyle);
		_featureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	Geometry* geom = _featureNode->getFeature()->getGeometry();

	geom->clear();

	_featureNode->setStyle(_polygonStyle);

	for (auto& n : points) {
		geom->push_back(n);
	}

	_featureNode->dirty();

	m_Property = new ToolProperty;

	m_Property->setNodeStyle(_polygonStyle);

	m_Property->setToolType(POLYGON);

	_polygonEdit = new FeatureEditor(_featureNode);
	_polygonEdit->setNodeMask(false);

	m_Property->setPoints(points);
	m_Property->setDragNode(_polygonEdit);
	_featureNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(_featureNode, _featureNode);

	_drawGroup->addChild(_polygonEdit);
	_drawGroup->addChild(_featureNode);

	return _featureNode;
}

std::vector<osg::Vec3d> DrawPolygonTool::getPoints()
{
	return _temPoint;
}

void DrawPolygonTool::setStyle(osgEarth::Style style)
{
	_polygonStyle = style;
}

void DrawPolygonTool::remove()
{
	if (_stippleFeatureNode)
	{
		_drawGroup->removeChild(_stippleFeatureNode);
		_stippleFeatureNode = NULL;
	}
	if (_featureNode)
	{
		_drawGroup->removeChild(_featureNode);
		_featureNode = NULL;
	}

}

NodeInfo DrawPolygonTool::getNodeinfos()
{

	NodeInfo info;
	if (_featureNode)
	{
		info.node = _featureNode;
		info.anneditr = _polygonEdit;
	}
	return info;
}
