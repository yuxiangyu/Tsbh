
#include "DrawRectangleTool.h"
#include <osgEarth/GeoMath>
#include <QDebug>
#include "AnnotationEditing.h"

using namespace osgEarth;


DrawRectangleTool::DrawRectangleTool(MapNode* mapNode, osg::Group* drawGroup)
    : DrawTool(mapNode, drawGroup)
    , _rectangleNode(nullptr)
{
    _rectangleStyle.getOrCreate<PolygonSymbol>()
        ->fill()
        ->color()
        = Color(Color::Yellow, 0.25);

    LineSymbol* ls = _rectangleStyle.getOrCreate<LineSymbol>();
    ls->stroke()->color() = Color::White;
    ls->stroke()->width() = 2.0f;
    ls->stroke()->widthUnits() = Units::PIXELS;

    //    RenderSymbol* render = _rectangleStyle.getOrCreate<RenderSymbol>();
    //    render->depthOffset()->enabled() = true;
    //    render->depthOffset()->automatic() = true;

    AltitudeSymbol* alt = _rectangleStyle.getOrCreate<AltitudeSymbol>();
    alt->clamping() = alt->CLAMP_TO_TERRAIN;
    alt->technique() = alt->TECHNIQUE_DRAPE;
}

void DrawRectangleTool::beginDraw(const osg::Vec3d& lla)
{
    //if (_rectangleNode) {
    //    GeoPoint end = GeoPoint(getMapNode()->getMapSRS(), lla);
    //    _rectangleNode->setCorner(getPointCorner(end), end);
    //    return;
    //}
    _vecPoints.push_back(lla);
    if (_vecPoints.size() < 1) {
        return;
    }

    if (!_rectangleNode) {
        GeoPoint start = GeoPoint(getMapNode()->getMapSRS(), _vecPoints[0]);
        //GeoPoint end = GeoPoint(getMapNode()->getMapSRS(), _vecPoints[1]);
        _rectangleNode = new RectangleNode( start,
            Distance(0, Units::KILOMETERS),
            Distance(0, Units::KILOMETERS),
            _rectangleStyle);
        //_rectangleNode->setCorner(getPointCorner(end), end);
        //drawCommand(osg::NodeList{_rectangleNode, new RectangleNodeEditor(_rectangleNode)});

		//_rectangleNode->setUserData(m_Property);

		 m_editr = new RectangleNodeEditor(_rectangleNode);
		 m_editr->setNodeMask(false);

		 osgEarth::Registry::objectIndex()->tagNode(_rectangleNode, _rectangleNode);

		_drawGroup->addChild(_rectangleNode);
		_drawGroup->addChild(m_editr);
    }
}

void DrawRectangleTool::moveDraw(const osg::Vec3d& v)
{
	if (_rectangleNode) {
		GeoPoint start = GeoPoint(getMapNode()->getMapSRS(), _vecPoints[0]);
		GeoPoint end = GeoPoint(getMapNode()->getMapSRS(), v);
		if (end.x() < start.x() && end.y() < start.y()) {
			_rectangleNode->setUpperRight(start);
			_rectangleNode->setLowerLeft(end);
		}
		else if (end.x() < start.x() && end.y() > start.y()) {
			_rectangleNode->setLowerRight(start);
			_rectangleNode->setUpperLeft(end);
		}
		else if (end.x() > start.x() && end.y() > start.y()) {
			_rectangleNode->setLowerLeft(start);
			_rectangleNode->setUpperRight(end);
		}
		else if (end.x() > start.x() && end.y() < start.y()) {
			_rectangleNode->setUpperLeft(start);
			_rectangleNode->setLowerRight(end);
		}
		//_rectangleNode->setCorner(getPointCorner(end), end);
		return;
	}
}

void DrawRectangleTool::endDraw(const osg::Vec3d& v)
{
	_vecPoints.clear();

	_vecPoints.push_back(_rectangleNode->getUpperLeft().vec3d());
	_vecPoints.push_back(_rectangleNode->getLowerLeft().vec3d());
	_vecPoints.push_back(_rectangleNode->getUpperRight().vec3d());
	_vecPoints.push_back(_rectangleNode->getLowerRight().vec3d());

	if (_rectangleNode) {

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(_rectangleStyle);

		m_Property->setToolType(RECTANGLE);

		m_Property->setPoints(_vecPoints);

		m_Property->setDragNode(m_editr);

		_rectangleNode->setUserData(m_Property);
	}
}

void DrawRectangleTool::resetDraw()
{
    _vecPoints.clear();
    _rectangleNode = nullptr;
}

/*************************************
 * Brief     : 静态绘制矩形
 * Parameter : std::vector<osg::Vec3d> points
			   points[0]: 左上角 经纬度
			   points[1]: 左下角 经纬度
			   points[2]: 右上角 经纬度
			   points[3]: 右下角 经纬度
 * Returns   : osg::ref_ptr<osg::Node>
 * Author    : WHL
*************************************/
osg::ref_ptr<osg::Node> DrawRectangleTool::draw(std::vector<osg::Vec3d> points)
{
	if (points.size() < 4) {return nullptr;	}

	//首先获取矩形的四个角用于计算长和宽
	GeoPoint UpperLeft = GeoPoint(getMapNode()->getMapSRS(), points[0]);	//左上角
	GeoPoint LowerLeft = GeoPoint(getMapNode()->getMapSRS(), points[1]);	//左下角
	GeoPoint UpperRight = GeoPoint(getMapNode()->getMapSRS(), points[2]);	//右上角
	GeoPoint LowerRight = GeoPoint(getMapNode()->getMapSRS(), points[3]);	//右下角

	double width = UpperLeft.distanceTo(UpperRight);						//矩形的长度
	double heigth = UpperLeft.distanceTo(LowerLeft);						//矩形的宽度
	
	//然后获取矩形的中心点
	double rightLon = points[3].x();										//矩形右边经度
	double leftLon = points[0].x();											//矩形左边经度
	double upperLat = points[0].y();										//矩形上边纬度
	double lowerLat = points[3].y();										//矩形下边纬度
	double centreLon = (rightLon + leftLon) / 2.0;							//矩形中心点经度
	double centreLat = (upperLat + lowerLat) / 2.0;							//矩形中心点纬度

	osg::Vec3d centreXYZ(centreLon, centreLat, 0.0);
	GeoPoint CentrePoint = GeoPoint(getMapNode()->getMapSRS(), centreXYZ);

	if (_rectangleNode == nullptr)
	{
		_rectangleNode = new RectangleNode(CentrePoint,		//第一个参数为矩形中心点的位置
			Distance(width, Units::METERS),
			Distance(heigth, Units::METERS),
			_rectangleStyle);
	}

	m_Property = new ToolProperty;
	m_Property->setNodeStyle(_rectangleStyle);
	m_Property->setToolType(RECTANGLE);
	m_editr = new RectangleNodeEditor(_rectangleNode);
	m_editr->setNodeMask(false);
	m_Property->setPoints(points);
	m_Property->setDragNode(m_editr);
	_rectangleNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(_rectangleNode, _rectangleNode);

	_drawGroup->addChild(_rectangleNode);
	_drawGroup->addChild(m_editr);

	return _rectangleNode;
}

void DrawRectangleTool::setStyle(osgEarth::Style style)
{
	_rectangleStyle = style;
}

void DrawRectangleTool::remove()
{
	if (_rectangleNode)
	{
		_vecPoints.clear();
		_drawGroup->removeChild(_rectangleNode);
		//_drawGroup->removeChild(new RectangleNodeEditor(_rectangleNode));
		_drawGroup->removeChild(m_editr);
		_rectangleNode = nullptr;
	}
}

NodeInfo DrawRectangleTool::getNodeinfos()
{
	NodeInfo info;
	if (_rectangleNode && m_editr)
	{
		info.node = _rectangleNode;
		info.anneditr = m_editr;
	}
	return info;
}

RectangleNode::Corner DrawRectangleTool::getPointCorner(const GeoPoint& corner)
{
    GeoPoint center = _rectangleNode->getPosition();
	//GeoPoint center = GeoPoint(getMapNode()->getMapSRS(), _vecPoints[0]);
    //Figure out the new width and height
#if 1
	//osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
	//而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
    double earthRadius = center.getSRS()->getEllipsoid()->getRadiusEquator();
#endif
#if 0
    //Add by gkk 20220223
    double earthRadius = center.getSRS()->getEllipsoid().getRadiusEquator();
#endif

    double lat = osg::DegreesToRadians(center.y());
    double lon = osg::DegreesToRadians(center.x());
    double halfWidthMeters = _rectangleNode->getWidth().as(Units::METERS) / 2.0;
    double halfHeightMeters = _rectangleNode->getHeight().as(Units::METERS) / 2.0;

    double eastLon, eastLat;
    double westLon, westLat;
    double northLon, northLat;
    double southLon, southLat;

    //Get the current corners
    GeoMath::destination(lat, lon, osg::DegreesToRadians(90.0), halfWidthMeters, eastLat, eastLon, earthRadius);
    GeoMath::destination(lat, lon, osg::DegreesToRadians(-90.0), halfWidthMeters, westLat, westLon, earthRadius);
    GeoMath::destination(lat, lon, osg::DegreesToRadians(0.0), halfHeightMeters, northLat, northLon, earthRadius);
    GeoMath::destination(lat, lon, osg::DegreesToRadians(180.0), halfHeightMeters, southLat, southLon, earthRadius);

	/* //保证所有路径都能有返回值，Note by gkk 20220216
    if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) > southLat) {
        return RectangleNode::CORNER_UPPER_LEFT;
    } else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) > southLat) {
        return RectangleNode::CORNER_UPPER_RIGHT;
    } else if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) < northLat) {
        return RectangleNode::CORNER_LOWER_LEFT;
    } else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) < northLat) {
        return RectangleNode::CORNER_LOWER_RIGHT;
    }
	*/

	RectangleNode::Corner ret_corner;
	if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) > southLat) {
		ret_corner = RectangleNode::CORNER_UPPER_LEFT;
	}
	else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) > southLat) {
		ret_corner = RectangleNode::CORNER_UPPER_RIGHT;
	}
	else if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) < northLat) {
		ret_corner = RectangleNode::CORNER_LOWER_LEFT;
	}
	else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) < northLat) {
		ret_corner = RectangleNode::CORNER_LOWER_RIGHT;
	}
	return ret_corner;
}
