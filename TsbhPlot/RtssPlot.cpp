#include "RtssPlot.h"
#include "simVis/Scenario.h"
#include "simVis/SceneManager.h"
#include "DrawTool.h"
#include "DrawLineTool.h"
#include "drawPolygonTool.h"
#include "drawrectangletool.h"
#include "GeoDiagonalArrow.h"
#include "geodoublearrow.h"
#include "GeoStraightArrow.h"
#include <osgEarth/GLUtils>
//...


RtssPlot::RtssPlot()
{
	this->plot = NULL;
	//	this->currentHandler1 = NULL;
	this->currentHandler = NULL;
}

RtssPlot::RtssPlot(simVis::View* view)
{
	this->view = view;
	//view2d = view2;

	group3dplot = new osg::Group();
	//group2dplot = new osg::Group();
	GLUtils::setGlobalDefaults(view->getSceneManager()->getMapNode()->getOrCreateStateSet());
	view->getSceneManager()->getMapNode()->addChild(group3dplot);
	//view2d->getSceneManager()->getScenario()->addChild(group2dplot);

	this->plot = NULL;
	//this->currentHandler1 = NULL;
	this->currentHandler = NULL;
}

void RtssPlot::setPlot(PlotStyle* plot)
{
	this->plot = plot;

	if (!currentHandler)
	{
		switch (plot->type)
		{

		case DrawTool::DrawType::DRAW_LINE:
			this->currentHandler = new DrawLineTool(view->getSceneManager()->getMapNode(), group3dplot);
			break;
		case DrawTool::DrawType::DRAW_POLYGON:
			//this->currentHandler = new DrawPolygonTool(mapNode, annGroup_3D);
			this->currentHandler = new DrawPolygonTool(view->getSceneManager()->getMapNode(), group3dplot);
			break;
		case DrawTool::DrawType::DRAW_RECTANGLE:
			//this->currentHandler = new DrawRectangleTool(mapNode, annGroup_3D);
			this->currentHandler = new DrawRectangleTool(view->getSceneManager()->getMapNode(), group3dplot);
			break;
		case DrawTool::DrawType::DRAW_DIAGONALARROW:
			//this->currentHandler = new GeoDiagonalArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoDiagonalArrow(view->getSceneManager()->getMapNode(), group3dplot);
			break;
		case DrawTool::DrawType::DRAW_DOUBLEARROW:
			//this->currentHandler = new GeoDoubleArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoDoubleArrow(view->getSceneManager()->getMapNode(), group3dplot);
			break;

		case DrawTool::DrawType::DRAW_STRAIGHTARROW:
			//this->currentHandler = new GeoStraightArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoStraightArrow(view->getSceneManager()->getMapNode(), group3dplot);
			break;
		}
	}
	view->addEventHandler(this->currentHandler);

}

bool RtssPlot::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (!plot)
		return false;
	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		if (this->currentHandler)
		{
			DrawTool* tool = dynamic_cast<DrawTool*>(this->currentHandler.get());
			tool->handle(ea, aa);
			view->removeEventHandler(currentHandler);
			this->currentHandler = NULL;
		}

		plot = NULL;
		return true;
	}
	return false;
}