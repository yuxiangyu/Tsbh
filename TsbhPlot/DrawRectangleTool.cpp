
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
 * Brief     : ��̬���ƾ���
 * Parameter : std::vector<osg::Vec3d> points
			   points[0]: ���Ͻ� ��γ��
			   points[1]: ���½� ��γ��
			   points[2]: ���Ͻ� ��γ��
			   points[3]: ���½� ��γ��
 * Returns   : osg::ref_ptr<osg::Node>
 * Author    : WHL
*************************************/
osg::ref_ptr<osg::Node> DrawRectangleTool::draw(std::vector<osg::Vec3d> points)
{
	if (points.size() < 4) {return nullptr;	}

	//���Ȼ�ȡ���ε��ĸ������ڼ��㳤�Ϳ�
	GeoPoint UpperLeft = GeoPoint(getMapNode()->getMapSRS(), points[0]);	//���Ͻ�
	GeoPoint LowerLeft = GeoPoint(getMapNode()->getMapSRS(), points[1]);	//���½�
	GeoPoint UpperRight = GeoPoint(getMapNode()->getMapSRS(), points[2]);	//���Ͻ�
	GeoPoint LowerRight = GeoPoint(getMapNode()->getMapSRS(), points[3]);	//���½�

	double width = UpperLeft.distanceTo(UpperRight);						//���εĳ���
	double heigth = UpperLeft.distanceTo(LowerLeft);						//���εĿ��
	
	//Ȼ���ȡ���ε����ĵ�
	double rightLon = points[3].x();										//�����ұ߾���
	double leftLon = points[0].x();											//������߾���
	double upperLat = points[0].y();										//�����ϱ�γ��
	double lowerLat = points[3].y();										//�����±�γ��
	double centreLon = (rightLon + leftLon) / 2.0;							//�������ĵ㾭��
	double centreLat = (upperLat + lowerLat) / 2.0;							//�������ĵ�γ��

	osg::Vec3d centreXYZ(centreLon, centreLat, 0.0);
	GeoPoint CentrePoint = GeoPoint(getMapNode()->getMapSRS(), centreXYZ);

	if (_rectangleNode == nullptr)
	{
		_rectangleNode = new RectangleNode(CentrePoint,		//��һ������Ϊ�������ĵ��λ��
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
	//osgEarth3.2�汾�� getEllipsoid()�������ص��� const Ellipsoid& ���ͣ�
	//����osgEarth3.1�汾�з��ص��� const osg::EllipsoidModel* ���� - Note by gkk 20220223
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

	/* //��֤����·�������з���ֵ��Note by gkk 20220216
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
