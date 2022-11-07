#include "TsbhPlot.h"
#include "DrawTool.h"
#include "DrawLineTool.h"
#include "drawPolygonTool.h"
#include "drawrectangletool.h"
#include "GeoDiagonalArrow.h"
#include "geodoublearrow.h"
#include "DrawCircleTool.h"
#include "GeoLune.h"
#include "GeoStraightArrow.h"
#include <osgEarth/GLUtils>
#include "DrawRunwayTool.h"
#include "DrawEightTool.h"
//...


TsbhPlot::TsbhPlot()
{
	this->plot = NULL;
//	this->currentHandler1 = NULL;
	this->currentHandler = NULL;
}

TsbhPlot::TsbhPlot(osgViewer::View* view, osgEarth::MapNode* node, bool mercator)
{
	this->view = view;
	//view2d = view2;

	group3dplot = new osg::Group();
	//group2dplot = new osg::Group();
	_node = node;
	GLUtils::setGlobalDefaults(_node->getOrCreateStateSet());
	_node->addChild(group3dplot);
	//view2d->getSceneManager()->getScenario()->addChild(group2dplot);

	this->plot = NULL;
	//this->currentHandler1 = NULL;
	this->currentHandler = NULL;
	this->bMercator = mercator;
}

void TsbhPlot::setPlot(PlotStyle* plot)
{
	this->plot = plot;

	if (!currentHandler)
	{
		switch (plot->type)
		{
		case DrawTool::DrawType::PLACE_MARK:
			break;
		case DrawTool::DrawType::DRAW_CIRCLE:
			this->currentHandler = new DrawCircleTool(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_GATHERINGPLACE:
			break;
		case DrawTool::DrawType::DRAW_GEOLUNE:
			break;
		case DrawTool::DrawType::DRAW_LINE:
			this->currentHandler = new DrawLineTool(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_POLYGON:
			//this->currentHandler = new DrawPolygonTool(mapNode, annGroup_3D);
			this->currentHandler = new DrawPolygonTool(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_RECTANGLE:
			//this->currentHandler = new DrawRectangleTool(mapNode, annGroup_3D);
			this->currentHandler = new DrawRectangleTool(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_DIAGONALARROW:
			//this->currentHandler = new GeoDiagonalArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoDiagonalArrow(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_DOUBLEARROW:
			//this->currentHandler = new GeoDoubleArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoDoubleArrow(_node, group3dplot);
			break;

		case DrawTool::DrawType::DRAW_STRAIGHTARROW:
			//this->currentHandler = new GeoStraightArrow(mapNode, annGroup_3D);
			this->currentHandler = new GeoStraightArrow(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_RUNWAY:
			//this->currentHandler = new GeoStraightArrow(mapNode, annGroup_3D);
			this->currentHandler = new DrawRunwayTool(_node, group3dplot);
			break;
		case DrawTool::DrawType::DRAW_EIGHT:
			//this->currentHandler = new GeoStraightArrow(mapNode, annGroup_3D);
			this->currentHandler = new DrawEightTool(_node, group3dplot);
			break;
		}
	}
	if (this->currentHandler)
	{
		DrawTool* tool = dynamic_cast<DrawTool*>(this->currentHandler.get());
		tool->isMercator() = bMercator;
		view->addEventHandler(this->currentHandler);
	}
	
}

bool TsbhPlot::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (!plot)
		return false;
	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE&&ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
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