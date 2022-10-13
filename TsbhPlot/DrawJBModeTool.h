#pragma once
#include "DrawTool.h"
#include <osgEarth/PlaceNode>
#include<osgEarth/ModelNode>
#include <osgEarth/Style>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osg/AutoTransform>

class TsbhPlot_EXPORT DrawJBModeTool :
	public DrawTool
{
public:
	DrawJBModeTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);
	~DrawJBModeTool();
	virtual DrawType getType() { return PLACE_MARK; }
	virtual ToolType getToolType() { return MARK; }
	virtual void beginDraw(const osg::Vec3d& lla);
	virtual void moveDraw(const osg::Vec3d& lla);
	virtual void endDraw(const osg::Vec3d& lla);
	virtual void resetDraw();
	virtual void remove();		//用于移除已添加的节点 - add by gkk 20220810

	osg::ref_ptr<osg::Node> draw(const osg::Vec3d & lla, JBModelType type, E_PlottingRole role);

	void setStyle(osgEarth::Style style);

	void setModelPath(JBModelType type, E_PlottingRole role);


	void initStyle();

	std::vector<osg::Vec3d> getPoints();

	void setModelPath(std::string path);

	std::string getModelPath()
	{
		return m_modelPath;
	}

	void setModelType(JBModelType type);

	JBModelType getModelType() {
		return m_type;
	}

	//void setImagePath(std::string image);

	NodeInfo getNodeinfos();
private:
	osg::Vec3d _centerPoint; 

	osg::ref_ptr<osg::Node> m_ModelNode;

	osg::ref_ptr<osgEarth::ModelNode> m_ModelPalceNode;

	osg::ref_ptr<GeoPositionNodeEditor>m_geoeditr;

	std::vector<osg::Vec3d> _temPoint;

	ToolProperty*  m_Property = NULL;

	osgEarth::MapNode* m_mapNode;

	osgEarth::Style m_modelStyle;

	std::string m_modelPath;

	JBModelType m_type;

	E_PlottingRole m_role;
};

