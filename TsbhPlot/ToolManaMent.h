#pragma once
#include "DrawTool.h"
#include "DrawPolygonTool.h"
#include "DrawLineTool.h"
#include "DrawTool.h"
#include "DrawCircleTool.h"
#include "DrawRectangleTool.h"
#include "DrawTextTool.h"
#include "DrawImageTool.h"
#include "DrawJBModeTool.h"
#include "ToolProperty.h"
#include "GeoGatheringPlace.h"
#include "SignageNode.h"
#include "DrawGeoPoint.h"
#include<qmap.h>
#include <osgViewer/View>

class TsbhPlot_EXPORT ToolManaMent
{
public:
	ToolManaMent(osgEarth::MapNode* mapNode, osg::Group* drawGroup, osgViewer::View* view);
	~ToolManaMent();
	//开始标绘类事件
	void StartToolHandler(ToolType type);

	//结束标绘类事件
	void RemoveToolHandler();

	//设置是否启动事件
	void setActive(bool isactive);

	DrawTool* getTool()
	{
		if (m_Tool) {
			return m_Tool.get();
		}
		return NULL;
	}
	//清空所有的标绘
	void ClearAllTool();
	//设置
	void setAllActive(bool b);
	//获取标绘节点
	QList<osg::ref_ptr<osg::Node>> getToolNodes(ToolType type);
	QList<osg::ref_ptr<osg::Node>> getAllNodes();

	//获取标绘节点
	void getToolNode(QString strId, osg::ref_ptr<osg::Node> &Node, osg::ref_ptr<osg::Node> &textNode);

	//获取标绘对象节点
	osg::ref_ptr<osg::Node> getToolNode(unsigned int objectid);
	//显隐
	void setNodeVisible(QString sID, bool bVisible);
	//定位
	void locationNode(QString sID);
	//删除
	void removeNode(QString sID);

	//绘制线
	osg::ref_ptr<osg::Node>  DrawStaticLine(std::vector<osg::Vec3d> points);

	//绘制圆
	osg::ref_ptr<osg::Node> DrawStaticCircle(osg::Vec3d cneterPoint, osg::Vec3d distancepoint);

	//绘制静态腰子图
	osg::ref_ptr<osg::Node> DrawStaticGeoGatheringPlace(std::vector<osg::Vec2> points);
	//绘制矩形
	osg::ref_ptr<osg::Node> DrawStaticRectangle(std::vector<osg::Vec3d> points);

	//绘制多边形
	osg::ref_ptr<osg::Node> DrawStaticPolygon(std::vector<osg::Vec3d> points);

	//绘制文字
	osg::ref_ptr<osg::Node> DrawStaticWord(osg::Vec3d cneterPoint,QString sText);

	//绘制点位
	osg::ref_ptr<osg::Node> DrawStaticPos(osg::Vec3d cneterPoint);

	//绘制图片
	osg::ref_ptr<osg::Node> DrawStaticPic(osg::Vec3d cneterPoint,std::string path);

	//绘制jBmodel
	osg::ref_ptr<osg::Node> DrawStaticJBModel(osg::Vec3d cneterPoint, JBModelType type, E_PlottingRole role);

	//绘制标绘文字
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name);

	//绘制标绘文字
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::Vec3d point, QString name);

	//绘制绘制标牌
	osg::ref_ptr<SignageNode>DrawSignageText(osg::ref_ptr<osg::Node> curNode, QString text = " ");

	//解决多边形和折线getbound()的中心点取到为空的问题
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec3d> points);

	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec2> points);
	int getNodenum(ToolType type);

	//设置模型路径
	QString getModelPath() { return m_sModelPath; }
	void setModelPath(QString sPath) { m_sModelPath = sPath; }

	//获取地形高度
	double getElevation(double lon, double lat);

	//设置文字标绘text，add by zzf,20220804
	void setText(QString sText) { m_sText = sText; }
	QString getText() { return m_sText; }
private:
	osg::ref_ptr<DrawTool> m_Tool;  //工具类
	osg::ref_ptr<MapNode>m_MapNode; //地图
	osg::ref_ptr<osg::Group>m_Root; //根节点
	QMap<ToolType, osg::ref_ptr<DrawTool>>m_Tools;
	osg::ref_ptr<osgViewer::View> m_View; //视图节点

	QString m_sModelPath;

	QString m_sText;//文字标绘
};

