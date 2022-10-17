#pragma once
#include "TsbhTools_global.h"
#include <osgEarth/MapNode>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include "TsbhTools.h"

class TsbhTools_EXPORT ToolHandle :public osgGA::GUIEventHandler
{
public:
	ToolHandle();
	~ToolHandle();
	ToolHandle(osgViewer::View* view,osgEarth::MapNode* node);
	void setTool(TsbhTools* tool1);
	void removeTool();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool getLocationAt(double x, double y, double& lon, double& lat, double& alt);

private:
	osgViewer::View* view;
	TsbhTools* tool;
	bool active;
	osg::ref_ptr<osgEarth::MapNode> _mapnode;
};

