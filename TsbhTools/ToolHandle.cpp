#include "ToolHandle.h"



ToolHandle::ToolHandle()
{
	tool = nullptr;
	active = false;
}


ToolHandle::~ToolHandle()
{
	//tool = nullptr;

}

ToolHandle::ToolHandle(osgViewer::View* view,osgEarth::MapNode* node)
{
	this->view = view;
	tool = nullptr;
	active = false;
	view->addEventHandler(this);
	_mapnode = node;
}

bool ToolHandle::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (!tool)
		return false;
	osg::Vec3d pos;
	bool su=getLocationAt( ea.getX(), ea.getY(), pos.x(), pos.y(), pos.z());
	if (!su)
		return false;
	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE&&ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		tool->onDoubleClick(new GeoPoint(_mapnode->getMap()->getSRS()->getGeographicSRS(), pos.x(),pos.y(),pos.z()));
		active = false;

		//view->removeEventHandler(this);
		delete tool;
		tool = nullptr;
		return true;
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH&&ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		tool->onLeftButton(new GeoPoint(_mapnode->getMap()->getSRS()->getGeographicSRS(), pos.x(), pos.y(), pos.z()));
		active = true;
		if (tool->m_bFinish == true)
		{
			active = false;
			delete tool;
			tool = nullptr;
			return true;
		}
		return true;
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE&&active)
	{
		tool->onMouseMove(new GeoPoint(_mapnode->getMap()->getSRS()->getGeographicSRS(), pos.x(), pos.y(), pos.z()));
		return true;
	}
	return false;

}

void ToolHandle::setTool(TsbhTools* tool1)
{
	tool = tool1;
}
void ToolHandle::removeTool()
{
	active = false;
	delete tool;
	tool = nullptr;
}


bool ToolHandle::getLocationAt( double x, double y, double& lon, double& lat, double& alt)
{
	osgUtil::LineSegmentIntersector::Intersections results;
	if (_mapnode &&  view->computeIntersections(x, y, results, 0x1)) {
		osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
		osg::Vec3d point = first.getWorldIntersectPoint();

		double lat_rad, lon_rad;
		_mapnode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
			point.x(), point.y(), point.z(), lat_rad, lon_rad, alt);

		lat = osg::RadiansToDegrees(lat_rad);
		lon = osg::RadiansToDegrees(lon_rad);
		if (alt <=0)
			alt = 0;
		return true;
	}
	return false;
}