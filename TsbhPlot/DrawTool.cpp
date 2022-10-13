#include "DrawTool.h"
#include <osg/Math>
#include <osgUtil/LineSegmentIntersector>
#include <osgEarth/TextSymbol>
#include <osgEarth/IconSymbol>
#include "FeatureEditing.h"
#include <qdebug.h>

DrawTool::DrawTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup, QObject* parent)
	: QObject(parent)
    , _mapNode(mapNode)
    , _drawGroup(drawGroup)
    , _active(true)
    , _dbClick(false)
    , _intersectionMask(0x1)
    , _tmpGroup(new osg::Group)
{
    _pnStyle.getOrCreate<osgEarth::IconSymbol>()->url()->setLiteral("Resources/placemark32.png");
    _pnStyle.getOrCreate<osgEarth::TextSymbol>()->size() = 14;
	_pnStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color() = Color(Color::Blue);
	_drawGroup->addChild(_tmpGroup);
}

bool DrawTool::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

    if (!_active)
        return false;

    _view = static_cast<osgViewer::View*>(aa.asView());

    const osgGA::GUIEventAdapter::EventType eventType = ea.getEventType();

    if (eventType == osgGA::GUIEventAdapter::KEYDOWN == eventType && ea.getKey() == osgGA::GUIEventAdapter::KEY_Escape) {
        resetDraw();
    }

    if (eventType == osgGA::GUIEventAdapter::KEYDOWN
           && (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) != 0) {
        if ( ea.getKey() == osgGA::GUIEventAdapter::KEY_Z) {
            CommandManager::instance()->undo();
            return true;
        } else if ( ea.getKey() == osgGA::GUIEventAdapter::KEY_X) {
            CommandManager::instance()->redo();
            return true;
        }
    }

    switch (ea.getEventType()) 
	{
    // 鼠标点击
    case osgGA::GUIEventAdapter::PUSH: 
	{
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) 
		{
            _mouseDownX = ea.getX();
            _mouseDownY = ea.getY();
			osg::Vec3d pos;
			getLocationAt(_view, ea.getX(), ea.getY(), pos.x(), pos.y(), pos.z());

			_pos = pos;
        }
        break;
    }
    // 鼠标移动
    case osgGA::GUIEventAdapter::MOVE: 
	{
        osg::Vec3d pos;
        getLocationAt(_view, ea.getX(), ea.getY(), pos.x(), pos.y(), pos.z());
        std::string coord  = osgEarth::Stringify ()<< pos.x() << ", " << pos.y() << " ," << pos.z();
        if (_coordPn.valid()) 
		{
            _coordPn->setPosition(osgEarth::GeoPoint::GeoPoint(getMapNode()->getMapSRS(), pos));
            _coordPn->setText(coord);
        }
		//pos.z() = 0.00;
        moveDraw(pos);
        aa.requestRedraw();
        break;
    }
    // 鼠标释放
    case osgGA::GUIEventAdapter::RELEASE: 
	{
        osg::Vec3d pos;
        getLocationAt(_view, ea.getX(), ea.getY(), pos.x(), pos.y(), pos.z());
		//pos.z() = 0.00;
        float eps = 1.0f;

        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) 
		{
            if (osg::equivalent(ea.getX(), _mouseDownX, eps) && osg::equivalent(ea.getY(), _mouseDownY, eps)) 
			{
                if (!_coordPn.valid()) 
				{
                    std::string coord  = osgEarth::Stringify ()<< pos.x() << " ," << pos.y() << " ," << pos.z();
                    //_coordPn = new osgEarth::PlaceNode( osgEarth::GeoPoint::GeoPoint(getMapNode()->getMapSRS(), pos), coord, _pnStyle);
                    //_tmpGroup->addChild(_coordPn);
                }
                beginDraw(pos);
                aa.requestRedraw();
            }
        } 
		else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) 
		{
            if (_coordPn.valid()) 
			{
                _tmpGroup->removeChild(_coordPn);
                _coordPn = NULL;
				this->setActive(false);
            }
	
			endDraw(pos);

			emit sigGetPoints();

			//beginDraw(pos);
			resetDraw();

            aa.requestRedraw();
        }
        break;
    }
    }

    return false;
}

ToolType DrawTool::getToolType()
{
	return ToolType();
}

void DrawTool::drawCommand(osg::Node *node)
{
    CommandManager::instance()->callCommand(new DrawCommand(_drawGroup, node));
}

void DrawTool::drawCommand(const osg::NodeList &nodes)
{
	//m_command = new DrawCommand(_drawGroup, nodes);
    CommandManager::instance()->callCommand(new DrawCommand(_drawGroup, nodes));
}

void DrawTool::RemoveCommand(const osg::NodeList &nodes)
{
	CommandManager::instance()->removeCommand(new DrawCommand(_drawGroup, nodes));
}

std::vector<osg::Vec3d> DrawTool::getPoints()
{
	return std::vector<osg::Vec3d>();
}

NodeInfo DrawTool::getNodeinfos()
{
	return NodeInfo();
}




bool DrawTool::getLocationAt(osgViewer::View* view, double x, double y, double& lon, double& lat, double& alt)
{
    osgUtil::LineSegmentIntersector::Intersections results;
    if (getMapNode() && view->computeIntersections(x, y, results, _intersectionMask)) {
        osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
        osg::Vec3d point = first.getWorldIntersectPoint();

#if 1
        double lat_rad, lon_rad;

		//osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
	    //而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
        getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
            point.x(), point.y(), point.z(), lat_rad, lon_rad, alt);

		lat = osg::RadiansToDegrees(lat_rad);
		lon = osg::RadiansToDegrees(lon_rad);

		////获取高程高度
		//double elevation = 0.0;
		//getMapNode()->getTerrain()->getHeight(getMapNode()->getMapSRS(), lat, lon, &elevation);
		//alt = elevation;
#endif

#if 0
        //Add by gkk 20220223
        //lla: lat lon alt(LL DEG + Alt M)
        osg::Vec3d lla = getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid().geocentricToGeodetic(point);

        //TODO: 待测试 - Add by gkk 20220223
        lat = lla[0];
        lon = lla[1];
        alt = lla[2];
#endif
		if (alt <= 0)
			alt = 1;
        return true;
    }
    return false;
}

DrawCommand::DrawCommand(osg::Group *parent, osg::Node *node)
    : parent_(parent) , node_(node), multi_(false){}

DrawCommand::DrawCommand(osg::Group *parent, osg::NodeList nodes)
    : parent_(parent) , nodes_(nodes), multi_(true){}

bool DrawCommand::execute() {
    if (multi_) {
        for (auto& node : nodes_)
            parent_->addChild(node);
        return true;
    } else {
        return parent_->addChild(node_);
    }
}

bool DrawCommand::unexecute() {
    if (multi_) {
		
		for (auto& node : nodes_)

			 parent_->removeChild(node);
			//node->setNodeMask(false);

		return true;
    } else {
        return parent_->removeChild(node_);
    }
	
}
