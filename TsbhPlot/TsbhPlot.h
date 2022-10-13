#pragma once

#include "TsbhPlot_global.h"
#include <osgEarth/MapNode>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include "PlotStyle.h"

class TsbhPlot_EXPORT TsbhPlot:public osgGA::GUIEventHandler
{
public:
	TsbhPlot();
	TsbhPlot(osgViewer::View* view,osgEarth::MapNode* node);
	void setPlot(PlotStyle* plot);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
	//osg::ref_ptr<osgViewer:ViewManager> views;
	osgViewer::View* view;
	//osgViewer:View* view2d;
	osg::Group* group3dplot;
	//osg::Group* group2dplot;
	osg::ref_ptr<osgGA::GUIEventHandler> currentHandler;
	//osg::ref_ptr<osgGA::GUIEventHandler> currentHandler1;
	//当前标绘类型
	PlotStyle* plot;
	osgEarth::MapNode* _node;
};
