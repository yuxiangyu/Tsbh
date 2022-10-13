#pragma once
#include "TsbhPlot_global.h"
#include <osgEarth/PlaceNode>
#include <osgEarth/CircleNode>
#include <osgEarth/MapNode>
#include <QVariant>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
#include "AnnotationEditing.h"

using namespace osgEarth;

typedef struct S_2DModelInfo_
{
	QString name;
	QString id;
	int type;
	std::string iconPath;
	//float scope;//摄像头监控范围
	//float startangle;//摄像头监控起始角度
	//float endangle;//摄像头监控结束角度
	double positionx;
	double positiony;
	osg::Vec3d position;
}S2DModelInfo;

class TsbhPlot_EXPORT CameraDevice
{
public:
	CameraDevice(osg::Group* group, osgEarth::MapNode* node);
	~CameraDevice();
	//在二维视图里创建模型
	void buildModel();
	//设置模型坐标点
	void setPosition(QString id,GeoPoint& gp);
	//设置模型数据
	void setModelDatas(QList<S2DModelInfo>values);
    //设置2维对应模型显影
	void set2DModelMask(QString id,bool visible);
	//设置2维模型显影
	void setModelsVisible(bool visble);
    //删除2D对应模型
	void removeOne(QString id);

	void removeAll();

	//获取模型图标路径
	std::string getIconPath(int iocnType);

	//获取指定ID的node - add by gkk 20220805
	osg::ref_ptr<osg::Node> getNode(QString nodeID);
private:
	osg::ref_ptr<osg::Group> mp_group;
	//osgEarth::Features::Feature* mFeature;
	//osg::ref_ptr<osgEarth::CircleNode> circle; //监控范围
	//QList<QMap<QString,osg::ref_ptr<osgEarth::CircleNode>>>m_circles;
	//osg::ref_ptr<osgEarth::PlaceNode> mLabel;
	QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>	m_Labels; //图片
	QList<S2DModelInfo>									m_modeldatas;
	osg::ref_ptr<osgEarth::MapNode>						mapNode;
	Style pm;
	Style m_circleStyle;
};

