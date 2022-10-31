#pragma once

#include <osgEarth/MapNode>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <osgEarth/Style>
#include <osgEarth/PlaceNode>
#include <QObject>
#include "CommandManager.h"
#include "AnnotationEditing.h"
#include <osgEarth/Registry>
#include <osgEarth/ObjectIndex>
#include "ToolProperty.h"
#include "TsbhPlot_global.h"


struct DrawCommand : public Command {
    DrawCommand(osg::Group* parent, osg::Node* node);
    DrawCommand(osg::Group* parent, osg::NodeList nodes);

    virtual bool execute();
    virtual bool unexecute();

    osg::ref_ptr<osg::Group> parent_;
    osg::ref_ptr<osg::Node> node_;
    osg::NodeList nodes_;
    bool multi_;
};

struct NodeInfo {
	osg::ref_ptr<osg::Node> node;
	osg::ref_ptr<AnnotationEditor>  anneditr;
};


class TsbhPlot_EXPORT DrawTool :public QObject,public osgGA::GUIEventHandler {
	Q_OBJECT
public:
	DrawTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup, QObject* parent = Q_NULLPTR);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	enum DrawType {
		DRAW_LINE, // 线
		DRAW_IMAGE,  //图片
		DRAW_CIRCLE, // 圈
		DRAW_TEXT,    // 文字
		DRAW_POLYGON, // 多边形
		DRAW_RECTANGLE, // 矩形
		MEASURE_HEIGHT, // 高度测量
		PLACE_MARK, // 点标记
		DRAW_POSITION, //点位
		IMAGE_OVERLAY, // 遮盖标记
		DRAW_ARROWED_LINE, // 箭头
		DRAW_BEZIER_CURVE, // 曲线
		RADIAL_LOS,
		MEASURE_AREA, // 面积测量
		MEASURE_DISTANCE, // 距离测量
		MEASURE_ANGLE, // 角度测量
		DRAW_STRAIGHTARROW, // 直箭头
		DRAW_DOUBLEARROW, // 双箭头
		DRAW_DIAGONALARROW, // 斜箭头
		DRAW_GATHERINGPLACE, // 聚居区
		DRAW_GEOLUNE, // 弓形
		DRAW_PARALLELSEARCH, //平行搜寻区
		DRAW_SECTORSEARCH, //扇形搜寻区
	};

    virtual DrawType getType() = 0;

	virtual ToolType getToolType();

    // 是否激活工具
    bool getActive() { return _active; }
    void setActive(bool on) { _active = on; }

    // 绘制Gorup
    osg::Group* getDrawGroup() { return _drawGroup; }
    osgEarth::MapNode* getMapNode() { return _mapNode; }

    void setMapNode(osgEarth::MapNode* mapNode) { _mapNode = mapNode; }

    virtual void beginDraw(const osg::Vec3d& lla) = 0;
    virtual void moveDraw(const osg::Vec3d& lla) = 0;
    virtual void endDraw(const osg::Vec3d& lla) = 0;
    virtual void resetDraw() = 0;
	virtual void remove() = 0;		//用于移除已添加的节点 - add by gkk 20220810


    void drawCommand(osg::Node* node);
    void drawCommand(const osg::NodeList& nodes);
	void RemoveCommand(const osg::NodeList &nodes);

	std::vector<osg::Vec3d> getPoints();

	struct NodeInfo getNodeinfos();

	bool &isMercator() { return _bMercator; }

public:
    // 获取点所在地理坐标
    bool getLocationAt(osgViewer::View* view, double x, double y, double& lon, double& lat, double& alt);
	//void removeobj()
//signals:
//	void sigPointInfo(osg::Vec3d);
	//void sigShow();
	//void PolyGonshow();
signals:
	void  sigGetPoints();

	void sigPoint(osg::Vec3d point);

protected:
    bool _active;
    bool _dbClick;
	bool _bIsDrawing = false;		//用于表示是否正在绘制中，开始绘制时置为true，结束绘制时置为false
    osgViewer::View* _view;
    osgEarth::MapNode* _mapNode;
    osg::Node::NodeMask _intersectionMask;
    osg::Group* _drawGroup;
    float _mouseDownX, _mouseDownY;
    osg::ref_ptr<osg::Group> _tmpGroup; // 临时绘制节点
    osgEarth::Style _pnStyle;
    std::vector<osg::Vec2> _controlPoints;
    std::vector<osg::Vec2> _drawParts;
    osg::ref_ptr<osgEarth::PlaceNode> _coordPn;
	osg::Vec3d _pos;
	DrawCommand *m_command = nullptr;
	bool _bMercator = false; // 是否为墨卡托坐标
};


