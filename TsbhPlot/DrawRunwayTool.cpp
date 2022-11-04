
#include "DrawRunwayTool.h"
#include <osgEarth/GeoMath>
#include <QDebug>
#include "AnnotationEditing.h"

using namespace osgEarth;


DrawRunwayTool::DrawRunwayTool(MapNode* mapNode, osg::Group* drawGroup)
    : DrawTool(mapNode, drawGroup)
    , _rectangleNode(nullptr)
{
    /*_rectangleStyle.getOrCreate<PolygonSymbol>()
        ->fill()
        ->color()
        = Color(Color::Yellow, 0.0);

    LineSymbol* ls = _rectangleStyle.getOrCreate<LineSymbol>();
    ls->stroke()->color() = Color(Color::White,0.0);
    ls->stroke()->width() = 0.0f;
    ls->stroke()->widthUnits() = Units::PIXELS;*/

    //    RenderSymbol* render = _rectangleStyle.getOrCreate<RenderSymbol>();
    //    render->depthOffset()->enabled() = true;
    //    render->depthOffset()->automatic() = true;

   /* AltitudeSymbol* alt = _rectangleStyle.getOrCreate<AltitudeSymbol>();
    alt->clamping() = alt->CLAMP_TO_TERRAIN;
    alt->technique() = alt->TECHNIQUE_DRAPE;*/

	_rectangleStyle2.getOrCreate<PolygonSymbol>()
		->fill()
		->color()
		= Color(Color::Yellow, 0.5);

	LineSymbol* ls2 = _rectangleStyle2.getOrCreate<LineSymbol>();
	ls2->stroke()->color() = osgEarth::Color::White;
	ls2->stroke()->width() = 1.0f;
	ls2->stroke()->widthUnits() = Units::PIXELS;

	//    RenderSymbol* render = _rectangleStyle.getOrCreate<RenderSymbol>();
	//    render->depthOffset()->enabled() = true;
	//    render->depthOffset()->automatic() = true;

	AltitudeSymbol* alt2 = _rectangleStyle2.getOrCreate<AltitudeSymbol>();
	alt2->clamping() = alt2->CLAMP_TO_TERRAIN;
	alt2->technique() = alt2->TECHNIQUE_DRAPE;

	_circleStyle1.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Color(osgEarth::Color::Yellow, 0.5);

	LineSymbol* ls3 = _circleStyle1.getOrCreate<LineSymbol>();
	ls3->stroke()->color() = osgEarth::Color::White;
	ls3->stroke()->width() = 1.0f;
	ls3->stroke()->widthUnits() = Units::PIXELS;

	//    RenderSymbol* render = _circleStyle.getOrCreate<RenderSymbol>();
	//    render->depthOffset()->enabled() = true;
	//    render->depthOffset()->automatic() = true;

	AltitudeSymbol* alt3 = _circleStyle1.getOrCreate<AltitudeSymbol>();
	alt3->verticalOffset() = 0.1;
	alt3->clamping() = alt3->CLAMP_TO_TERRAIN;
	alt3->technique() = alt3->TECHNIQUE_DRAPE;

	//_circleStyle2.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Color(osgEarth::Color::Yellow, 0.5);

	//LineSymbol* ls4 = _circleStyle2.getOrCreate<LineSymbol>();
	//ls4->stroke()->color() = osgEarth::Color::White;
	//ls4->stroke()->width() = 1.0f;
	//ls4->stroke()->widthUnits() = Units::PIXELS;

	////    RenderSymbol* render = _circleStyle.getOrCreate<RenderSymbol>();
	////    render->depthOffset()->enabled() = true;
	////    render->depthOffset()->automatic() = true;

	//AltitudeSymbol* alt4 = _circleStyle2.getOrCreate<AltitudeSymbol>();
	//alt4->verticalOffset() = 0.1;
	//alt4->clamping() = alt4->CLAMP_TO_TERRAIN;
	//alt4->technique() = alt4->TECHNIQUE_DRAPE;
	_rectangleNode2 = nullptr;
	
}

void DrawRunwayTool::beginDraw(const osg::Vec3d& lla)
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
            _rectangleStyle2);
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

void DrawRunwayTool::moveDraw(const osg::Vec3d& v)
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

void DrawRunwayTool::endDraw(const osg::Vec3d& v)
{
	_vecPoints.clear();

	_vecPoints.push_back(_rectangleNode->getUpperLeft().vec3d());
	_vecPoints.push_back(_rectangleNode->getLowerLeft().vec3d());
	_vecPoints.push_back(_rectangleNode->getUpperRight().vec3d());
	_vecPoints.push_back(_rectangleNode->getLowerRight().vec3d());

	width = _rectangleNode->getUpperLeft().distanceTo(_rectangleNode->getUpperRight());						//矩形的长度
	heigth = _rectangleNode->getUpperLeft().distanceTo(_rectangleNode->getLowerLeft());						//矩形的宽度

	/*if (_rectangleNode) {

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(_rectangleStyle);

		m_Property->setToolType(RECTANGLE);

		m_Property->setPoints(_vecPoints);

		m_Property->setDragNode(m_editr);

		_rectangleNode->setUserData(m_Property);
	}*/

	//画半圆
	//double r1 = getRadias();
	
	if (!_circleNode1.valid())
	
		_circleNode1 = new CircleNode();
	
	if (!_circleNode2.valid())
		_circleNode2 = new CircleNode();
//	_circleNode1->setStyle(_circleStyle1);
	//_circleNode2->setStyle(_circleStyle2);
	
	buildConerPoint();
	_drawGroup->addChild(_rectangleNode2);
	_drawGroup->addChild(_circleNode1);
	_drawGroup->addChild(_circleNode2);
	_drawGroup->removeChild(_rectangleNode);
}

double DrawRunwayTool::getRadias()
{
	double distance1 = width;
	double distance2 = heigth;

	double distance = distance1 > distance2 ? distance2 : distance1;

	return distance / 2;
}

double DrawRunwayTool::getAngle()
{
	bool bb = width > heigth ? true : false;

	double angle_start;
	if(bb)
		angle_start = osg::RadiansToDegrees(GeoMath::bearing(osg::DegreesToRadians(_rectangleNode->getUpperLeft().y()), osg::DegreesToRadians(_rectangleNode->getUpperLeft().x()), osg::DegreesToRadians(_rectangleNode->getLowerLeft().y()), osg::DegreesToRadians(_rectangleNode->getLowerLeft().x())));
	else
		angle_start = osg::RadiansToDegrees(GeoMath::bearing(osg::DegreesToRadians(_rectangleNode->getUpperRight().y()), osg::DegreesToRadians(_rectangleNode->getUpperRight().x()), osg::DegreesToRadians(_rectangleNode->getUpperLeft().y()), osg::DegreesToRadians(_rectangleNode->getUpperLeft().x())));
	return angle_start;
}

//获取中点坐标
void DrawRunwayTool::buildConerPoint()
{
	bool bb = width > heigth ? true : false;
	double r = getRadias();
	double angle = getAngle();
	if (bb)
	{
		double delta = r / width;
		double newx = _rectangleNode->getUpperLeft().x() + delta*(_rectangleNode->getUpperRight().x() - _rectangleNode->getUpperLeft().x());
		double newy = _rectangleNode->getUpperLeft().y() + delta*(_rectangleNode->getUpperRight().y() - _rectangleNode->getUpperLeft().y());
		double newz = _rectangleNode->getUpperLeft().alt() + delta*(_rectangleNode->getUpperRight().alt() - _rectangleNode->getUpperLeft().alt());
		double newx1 = _rectangleNode->getUpperRight().x() + delta*(_rectangleNode->getUpperLeft().x() - _rectangleNode->getUpperRight().x());
		double newy1 = _rectangleNode->getUpperRight().y() + delta*(_rectangleNode->getUpperLeft().y() - _rectangleNode->getUpperRight().y());
		double newz1 = _rectangleNode->getUpperRight().alt() + delta*(_rectangleNode->getUpperLeft().alt() - _rectangleNode->getUpperRight().alt());
		double newx2 = _rectangleNode->getLowerLeft().x() + delta*(_rectangleNode->getLowerRight().x() - _rectangleNode->getLowerLeft().x());
		double newy2 = _rectangleNode->getLowerLeft().y() + delta*(_rectangleNode->getLowerRight().y() - _rectangleNode->getLowerLeft().y());
		double newz2 = _rectangleNode->getLowerLeft().alt() + delta*(_rectangleNode->getLowerRight().alt() - _rectangleNode->getLowerLeft().alt());
		double newx3 = _rectangleNode->getLowerRight().x() + delta*(_rectangleNode->getLowerLeft().x() - _rectangleNode->getLowerRight().x());
		double newy3 = _rectangleNode->getLowerRight().y() + delta*(_rectangleNode->getLowerLeft().y() - _rectangleNode->getLowerRight().y());
		double newz3 = _rectangleNode->getLowerRight().alt() + delta*(_rectangleNode->getLowerLeft().alt() - _rectangleNode->getLowerRight().alt());
		
		if (!_rectangleNode2)
			_rectangleNode2 = new RectangleNode(GeoPoint(getMapNode()->getMapSRS(), (newx + newx3) / 2, (newy + newy3) / 2, (newz + newz3) / 2),
				Distance(width-2*r, Units::METERS),
				Distance(heigth, Units::METERS),
				_rectangleStyle2);
		else
		{
			_rectangleNode2->setUpperLeft(GeoPoint(getMapNode()->getMapSRS(), newx, newy, newz));
			_rectangleNode2->setUpperRight(GeoPoint(getMapNode()->getMapSRS(), newx1, newy1, newz1));
			_rectangleNode2->setLowerLeft(GeoPoint(getMapNode()->getMapSRS(), newx2, newy2, newz2));
			_rectangleNode2->setLowerRight(GeoPoint(getMapNode()->getMapSRS(), newx3, newy3, newz3));
		}
		_circleNode1->set(
			GeoPoint::GeoPoint(getMapNode()->getMapSRS(), (newx + newx2) / 2, (newy + newy2)/2, (newz + newz2) / 2),
			Distance(r, Units::METERS), _circleStyle1,
			Angle(angle, Units::DEGREES), Angle(angle + 180, Units::DEGREES),false);
		//_circleNode1->setPosition(GeoPoint(getMapNode()->getMapSRS(),(newx+newx2)/2,(newy+newy2),(newz+newz2)/2));
		//_circleNode1->setRadius(osgEarth::Distance(r, Units::METERS));
		//_circleNode1->setArcStart(Angle(angle, Units::DEGREES));
		//_circleNode1->setArcEnd(Angle(angle+180, Units::DEGREES));

		/*_circleNode2->setPosition(GeoPoint(getMapNode()->getMapSRS(), (newx1 + newx3) / 2, (newy1 + newy3), (newz1 + newz3) / 2));
		_circleNode2->setRadius(osgEarth::Distance(r, Units::METERS));
		_circleNode2->setArcStart(Angle(angle+180, Units::DEGREES));
		_circleNode2->setArcEnd(Angle(angle + 360, Units::DEGREES));*/
		_circleNode2->set(
			GeoPoint::GeoPoint(getMapNode()->getMapSRS(), (newx1 + newx3) / 2, (newy1 + newy3)/2, (newz1 + newz3) / 2),
			Distance(r, Units::METERS), _circleStyle1,
			Angle(angle + 180, Units::DEGREES), Angle(angle + 360, Units::DEGREES), false);
	}
	else
	{
		double delta = r / heigth;
		double newx = _rectangleNode->getUpperLeft().x() + delta*(_rectangleNode->getLowerLeft().x() - _rectangleNode->getUpperLeft().x());
		double newy = _rectangleNode->getUpperLeft().y() + delta*(_rectangleNode->getLowerLeft().y() - _rectangleNode->getUpperLeft().y());
		double newz = _rectangleNode->getUpperLeft().z() + delta*(_rectangleNode->getLowerLeft().z() - _rectangleNode->getUpperLeft().z());
		double newx1 = _rectangleNode->getUpperRight().x() + delta*(_rectangleNode->getLowerRight().x() - _rectangleNode->getUpperRight().x());
		double newy1 = _rectangleNode->getUpperRight().y() + delta*(_rectangleNode->getLowerRight().y() - _rectangleNode->getUpperRight().y());
		double newz1 = _rectangleNode->getUpperRight().z() + delta*(_rectangleNode->getLowerRight().z() - _rectangleNode->getUpperRight().z());
		double newx2 = _rectangleNode->getLowerLeft().x() + delta*(_rectangleNode->getUpperLeft().x() - _rectangleNode->getLowerLeft().x());
		double newy2 = _rectangleNode->getLowerLeft().y() + delta*(_rectangleNode->getUpperLeft().y() - _rectangleNode->getLowerLeft().y());
		double newz2 = _rectangleNode->getLowerLeft().z() + delta*(_rectangleNode->getUpperLeft().z() - _rectangleNode->getLowerLeft().z());
		double newx3 = _rectangleNode->getLowerRight().x() + delta*(_rectangleNode->getUpperRight().x() - _rectangleNode->getLowerRight().x());
		double newy3 = _rectangleNode->getLowerRight().y() + delta*(_rectangleNode->getUpperRight().y() - _rectangleNode->getLowerRight().y());
		double newz3 = _rectangleNode->getLowerRight().z() + delta*(_rectangleNode->getUpperRight().z() - _rectangleNode->getLowerRight().z());

		if (!_rectangleNode2)
		{ 
			_rectangleNode2 = new RectangleNode(GeoPoint(getMapNode()->getMapSRS(), (newx + newx3) / 2, (newy + newy3) / 2, (newz + newz3) / 2),
				Distance(width, Units::METERS),
				Distance(heigth-2*r, Units::METERS),
				_rectangleStyle2);
		}
		else
		{ 
		_rectangleNode2->setUpperLeft(GeoPoint(getMapNode()->getMapSRS(), newx, newy, newz));
		_rectangleNode2->setUpperRight(GeoPoint(getMapNode()->getMapSRS(), newx1, newy1, newz1));
		_rectangleNode2->setLowerLeft(GeoPoint(getMapNode()->getMapSRS(), newx2, newy2, newz2));
		_rectangleNode2->setLowerRight(GeoPoint(getMapNode()->getMapSRS(), newx3, newy3, newz3));
		}
		//_circleNode1->setPosition(GeoPoint(getMapNode()->getMapSRS(), (newx + newx2) / 2, (newy + newy2), (newz + newz2) / 2));
		//_circleNode1->setRadius(osgEarth::Distance(r, Units::METERS));
		//_circleNode1->setArcStart(Angle(angle, Units::DEGREES));
		//_circleNode1->setArcEnd(Angle(angle + 180, Units::DEGREES));

		_circleNode1->set(
			GeoPoint::GeoPoint(getMapNode()->getMapSRS(), (newx + newx1) / 2, (newy + newy1) / 2, (newz + newz1) / 2),
			Distance(r, Units::METERS), _circleStyle1,
			Angle(angle, Units::DEGREES), Angle(angle + 180, Units::DEGREES), false);

	//	_circleNode2->setPosition(GeoPoint(getMapNode()->getMapSRS(), (newx1 + newx3) / 2, (newy1 + newy3), (newz1 + newz3) / 2));
	//	_circleNode2->setRadius(osgEarth::Distance(r, Units::METERS));
	//	_circleNode2->setArcStart(Angle(angle + 180, Units::DEGREES));
	//	_circleNode2->setArcEnd(Angle(angle + 360, Units::DEGREES));
		_circleNode2->set(
			GeoPoint::GeoPoint(getMapNode()->getMapSRS(), (newx2 + newx3) / 2, (newy2 + newy3) / 2, (newz2 + newz3) / 2),
			Distance(r, Units::METERS), _circleStyle1,
			Angle(angle + 180, Units::DEGREES), Angle(angle + 360, Units::DEGREES), false);
	}
}

void DrawRunwayTool::resetDraw()
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
osg::ref_ptr<osg::Node> DrawRunwayTool::draw(std::vector<osg::Vec3d> points)
{
	if (points.size() < 4) {return nullptr;	}

	//首先获取矩形的四个角用于计算长和宽
	GeoPoint UpperLeft = GeoPoint(getMapNode()->getMapSRS(), points[0]);	//左上角
	GeoPoint LowerLeft = GeoPoint(getMapNode()->getMapSRS(), points[1]);	//左下角
	GeoPoint UpperRight = GeoPoint(getMapNode()->getMapSRS(), points[2]);	//右上角
	GeoPoint LowerRight = GeoPoint(getMapNode()->getMapSRS(), points[3]);	//右下角

	width = UpperLeft.distanceTo(UpperRight);						//矩形的长度
	heigth = UpperLeft.distanceTo(LowerLeft);						//矩形的宽度
	
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
			_rectangleStyle2);
	}

	m_Property = new ToolProperty;
	m_Property->setNodeStyle(_rectangleStyle2);
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

void DrawRunwayTool::setStyle(osgEarth::Style style)
{
	_rectangleStyle2 = style;
}

void DrawRunwayTool::remove()
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

NodeInfo DrawRunwayTool::getNodeinfos()
{
	NodeInfo info;
	if (_rectangleNode && m_editr)
	{
		info.node = _rectangleNode;
		info.anneditr = m_editr;
	}
	return info;
}

RectangleNode::Corner DrawRunwayTool::getPointCorner(const GeoPoint& corner)
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
