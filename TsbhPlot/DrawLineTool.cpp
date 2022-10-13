#include "DrawLineTool.h"

//#include <QDebug>

#define LC "[DrawLineTool] "

using namespace osgEarth;


DrawLineTool::DrawLineTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup)
    : DrawTool(mapNode, drawGroup)
    , _feature(0)
    , _featureNode(0)
    , _stippleFeature(0)
    , _stippleFeatureNode(0)

{
    _vecPoint.clear();


	AltitudeSymbol* alt = _stippleLineStyle.getOrCreate<AltitudeSymbol>();
	alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	//alt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
	//alt->technique() = AltitudeSymbol::TECHNIQUE_GPU;
	alt->technique() = AltitudeSymbol::TECHNIQUE_MAP;
	alt->verticalOffset() = 0.1;
	// offset to mitigate Z fighting
	RenderSymbol* render = _stippleLineStyle.getOrCreate<RenderSymbol>();
	render->lighting() = false;
	render->depthOffset()->enabled() = true;
	render->depthOffset()->automatic() = true;

	// define a style for the line
	LineSymbol* s_ls = _stippleLineStyle.getOrCreate<LineSymbol>();
	s_ls->stroke()->color() = Color(0x13f8bf7f, osgEarth::Color::RGBA);
	s_ls->stroke()->width() = 2.0f;
	s_ls->stroke()->widthUnits() = Units::PIXELS;
	s_ls->stroke()->smooth() = true;
	//ls->stroke()->stipple() = 255;
	s_ls->tessellation() = 5;

	//m_TextStyle.getOrCreate<TextSymbol>()->size() = 18.0f;
	//m_TextStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	//m_TextStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	//m_TextStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	//m_TextStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	//m_TextStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

   
    // clamp to the terrain skin as it pages in
    AltitudeSymbol* lalt = _lineStyle.getOrCreate<AltitudeSymbol>();
	lalt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;				//贴地，线不是直线
	//lalt->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;		//不显示
	//lalt->clamping() = AltitudeSymbol::CLAMP_NONE;					//不贴地
	//lalt->clamping() = AltitudeSymbol::CLAMP_ABSOLUTE;				//不贴地
	//lalt->technique() = AltitudeSymbol::TECHNIQUE_GPU;				//运动时线会抖动
	lalt->technique() = AltitudeSymbol::TECHNIQUE_MAP;					//运动时线不会抖动
	//lalt->technique() = AltitudeSymbol::TECHNIQUE_SCENE;				//运动时线不会抖动
	//lalt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;				//画出的线会变粗
	lalt->verticalOffset() = 0.1;
    // offset to mitigate Z fighting
    //RenderSymbol* render = _stippleLineStyle.getOrCreate<RenderSymbol>();
    //render->lighting() = false;
    //render->depthOffset()->enabled() = true;
    //render->depthOffset()->automatic() = true;

	RenderSymbol* render2 = _lineStyle.getOrCreate<RenderSymbol>();
	render2->lighting() = false;
	render2->depthOffset()->enabled() = true;
	render2->depthOffset()->automatic() = true;

    // define a style for the line
    LineSymbol* ls = _lineStyle.getOrCreate<LineSymbol>();
	ls->stroke()->color() = Color(0x13f8bf7f, osgEarth::Color::RGBA);
	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->stroke()->smooth() = true;
	//ls->stroke()->stipple() = 255;
	ls->tessellation() = 5;



	//_lineStyle.getOrCreate<IconSymbol>()->declutter() = true;
	//_lineStyle.getOrCreate<IconSymbol>()->alignment() = IconSymbol::ALIGN_CENTER_CENTER;
	//_lineStyle.getOrCreate<IconSymbol>()->scale() = 0.5;
	//_lineStyle.getOrCreate<IconSymbol>()->url()->setLiteral("C:/Users/hasee/Desktop/whlpro/630whlpro/bin/testImage/1.jpeg");
}

void DrawLineTool::ReadIniStyle()
{

}

void DrawLineTool::beginDraw(const osg::Vec3d& lla)
{
    _vecPoint.push_back(lla);
    if (_vecPoint.size() <= 1) {
        return;
    }
    if (_featureNode == 0) {
        _feature = new Feature(
            new LineString(),
            getMapNode()->getMapSRS()->getGeographicSRS());
        _featureNode = new FeatureNode(
             _feature, _lineStyle);

		osgEarth::Registry::objectIndex()->tagNode(_featureNode, _featureNode);
		_drawGroup->addChild(_featureNode);
	
    }
    _feature->getGeometry()->clear();

    _featureNode->setStyle(_lineStyle);
 

    for (auto& n : _vecPoint) {
        _feature->getGeometry()->push_back(n);
    }

    _featureNode->dirty();

    if (_stippleFeatureNode != 0) {
        _stippleFeature->getGeometry()->clear();
    }
}

void DrawLineTool::moveDraw(const osg::Vec3d& lla)
{
    if (_vecPoint.size() <= 0) {
        return;
    }
    if (_stippleFeatureNode == 0) {
        _stippleFeature = new Feature( new LineString, getMapNode()->getMapSRS()->getGeographicSRS());
        _stippleFeatureNode = new FeatureNode(_stippleFeature, _stippleLineStyle);

		_drawGroup->addChild(_stippleFeatureNode);
    }

    _stippleFeature->getGeometry()->clear();
    _stippleFeature->getGeometry()->push_back(_vecPoint[_vecPoint.size() - 1]);
    _stippleFeature->getGeometry()->push_back(lla);

	_stippleFeatureNode->dirty();
}

void DrawLineTool::endDraw(const osg::Vec3d& lla)
{
	_temPoint.clear();
	_temPoint = _vecPoint;
	if (_featureNode) {

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(_lineStyle);

		m_Property->setToolType(LINE);

		_featuredit = new FeatureEditor(_featureNode);
		_featuredit->setNodeMask(false);
		_drawGroup->addChild(_featuredit);

		m_Property->setPoints(_vecPoint);
		m_Property->setCenterPoint(_featureNode->getBound().center());
		m_Property->setDragNode(_featuredit);
		_featureNode->setUserData(m_Property);
	}
}

void DrawLineTool::resetDraw()
{
    _vecPoint.clear();
    if (_stippleFeatureNode != 0) {
		//_drawGroup->removeChild(_stippleFeatureNode);
        _stippleFeature->getGeometry()->clear();
    }
	_stippleFeature = nullptr;
	_stippleFeatureNode = nullptr;
	_featureNode = nullptr;
}

void DrawLineTool::setStartPoint(const osg::Vec3d & lla)
{
	_vecPoint.push_back(lla);
}

osg::ref_ptr<osg::Node> DrawLineTool::draw(std::vector<osg::Vec3d> points)
{
	////test
	//points.clear();
	//points.push_back(osg::Vec3d(114.357, 30.8641, 24.3622));
	//points.push_back(osg::Vec3d(114.355, 30.8614, 22.0389));
	//points.push_back(osg::Vec3d(114.359, 30.8619, 24.3865));
	//points.push_back(osg::Vec3d(114.356, 30.8601, 21.571));

	osg::ref_ptr<osg::Group>  group = new osg::Group();
	if (points.size() < 2)
		return NULL;
	_feature = new Feature(new osgEarth::LineString(), getMapNode()->getMapSRS()->getGeographicSRS());
	_feature->geoInterp() = GEOINTERP_RHUMB_LINE;
	_featureNode = new FeatureNode(_feature, _lineStyle);
	for each(auto item in points)
	{
		//GeoPoint point_tmp = GeoPoint(getMapNode()->getMapSRS(), item);
		//osg::Vec3d vec3d_pos(point_tmp.x(), point_tmp.y(), point_tmp.z());
		//_feature->getGeometry()->push_back(vec3d_pos);

		//toWorldPoint(GeoPoint(mapSRS, startline), startWorld);
		_feature->getGeometry()->push_back(item);
	}

	_featuredit = new FeatureEditor(_featureNode);
	_featuredit->setNodeMask(false);
	_drawGroup->addChild(_featuredit);

	m_Property = new ToolProperty;
	m_Property->setNodeStyle(_lineStyle);

	m_Property->setToolType(LINE);

	m_Property->setPoints(points);

	m_Property->setDragNode(_featuredit);

	_featureNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(_featureNode, _featureNode);

	_drawGroup->addChild(_featureNode);

	////test
	//osgEarth::Geometry* p_geo = _featureNode->getFeature()->getGeometry();
	//qDebug() << "PointCount: " << p_geo->getTotalPointCount();
	//std::vector<osg::Vec3d> vec_vec = p_geo->asVector();
	//for (int i = 0; i < vec_vec.size(); i++)
	//{
	//	osg::Vec3d vec_point = vec_vec.at(i);
	//	qDebug() << "Vec3d    : " << vec_point.x() << vec_point.y() << vec_point.z();
	//}

	return _featureNode;
}

void DrawLineTool::setStyle(osgEarth::Style style)
{
	_lineStyle = style;
}

NodeInfo DrawLineTool::getNodeinfos()
{
	NodeInfo info;
	if (_featureNode)
	{
		info.node = _featureNode;
		info.anneditr = _featuredit;
	}
	_drawGroup->removeChild(_stippleFeatureNode);
	return info;
}

std::vector<osg::Vec3d> DrawLineTool::getPoints()
{
	return _temPoint;
}

void DrawLineTool::remove()
{
	_vecPoint.clear();
	if (_stippleFeatureNode)
	{
		_stippleFeature->getGeometry()->clear();

		_drawGroup->removeChild(_featureNode);
		_drawGroup->removeChild(_stippleFeatureNode);
		_stippleFeature = nullptr;
		_stippleFeatureNode = nullptr;
		_featureNode = nullptr;
	}
}
