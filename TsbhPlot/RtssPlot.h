#pragma once

#include "rtssplot_global.h"
#include <osgEarth/MapNode>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <simVis/SceneManager.h>
#include <simVis/Utils.h>
#include <simVis/Viewer.h>
#include "PlotStyle.h"

class RTSSPLOT_EXPORT RtssPlot :public osgGA::GUIEventHandler
{
public:
	RtssPlot();
	RtssPlot(simVis::View* view);
	void setPlot(PlotStyle* plot);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
	//osg::ref_ptr<simVis::ViewManager> views;
	simVis::View* view;
	//simVis::View* view2d;
	osg::Group* group3dplot;
	//osg::Group* group2dplot;
	osg::ref_ptr<osgGA::GUIEventHandler> currentHandler;
	//osg::ref_ptr<osgGA::GUIEventHandler> currentHandler1;
	//当前标绘类型
	PlotStyle* plot;
};

