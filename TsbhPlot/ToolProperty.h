#pragma once
#include <osg/Referenced>
#include <osgEarth/Style>
#include <osgEarth/PlaceNode>
#include<osg/ref_ptr>
#include<qstring.h>
#include "SignageNode.h"
#include "AnnotationEditing.h"
#include "TsbhPlot_global.h"

//struct NodeInfo
//{
//	std::string strId;            //标绘id
//	std::string name;             //标绘名字
//	osg::ref_ptr<osg::Node> node; //标绘节点
//	unsigned int objectid;         //对象id
//	osg::ref_ptr<AnnotationEditor>  anneditr; //标绘动态点
//	std::vector<osg::Vec3d> points;  //坐标点
//	std::string miaoShu;
//	bool biaotou;
//};

enum ToolType
{
	LINE,			// 线
	IMAGE,			//图片
	CIRCLE,			// 圈
	TEXT,			// 文字
	POLYGON,		// 多边形
	RECTANGLE,		// 矩形
	MARK,			// 点标记
	GEOGETHER,		//腰子图
	GEOPOSITION,	//发射点位
	RUNWAY,
};

enum JBModelType
{
	//!!!注意!!!新增的标绘类型一定要靠后放，放在 NOTYPE 前一个
	ZXKMODEL,   //中心库
	DJKMODEL,  //待机库
	FSCDMODEL, //发射阵地
	ZBYCMODEL, //战备油区
	DJQYMODEL, //待机区域
	YQMODEL,   //营区
	GFJWMODEL, //国防机务
	GLSKMODEL,
	TXGLMODEL,  //通信光栏
	DXDDZXK,      //地下导弹中心库
	ZHC,        //指挥车
	DDFSZD,     //导弹发射阵地
	ZCC,       //侦察车
	DXDDZXC,   //地下导弹中心库
	ZYZD,      //转载阵地
	DSGDTXSN,  //地上固定通信枢纽
	DDFSC,     //导弹发射车
	GCZBBZC,   //工程装备保障车
	TXC,       //通信车
	QC,        //汽车
	TZC,       //特装车
	LJYCDKQBLD, //陆基远程对空情报雷达
	LDQBZ,      //雷达情报站
	FKBQZD,     //防空兵群阵地
	YJJ,        //预警机
	HZJ,        //轰炸机
	ZDJ,        //战斗机 
	HKMJ,       //航空母舰
	JB_HQBZBD,	//后勤保障部（分）队、群
	JB_ZBBZBD,	//装备保障部（分）队、群
	JB_YLJHZ,	//医疗救护组

	//一定要放在最后
	NOTYPE,		//无类型，用于判断是否正在进行标号绘制
};

//标绘角色-红、蓝 - add by gkk 20220516
enum class E_PlottingRole : int {
	RED = 0,			//表示红方
	BLUE				//表示蓝方
};

class TsbhPlot_EXPORT ToolProperty : public osg::Referenced
{
public:
	ToolProperty();
	~ToolProperty();
	//设置标绘样式类型
	void setNodeStyle(osgEarth::Style style);
	//获取标绘样式类型
	osgEarth::Style getNodeStyle() {
		return m_NodeStyle;
	}
	//设置名称
	void setToolName(QString name);
	//获取名称
	QString getToolName() {
		return m_name;
	}
	//设置标绘类型
	void setToolType(ToolType type);

	//获取样式类型
	ToolType getToolType() {
		return m_type;
	}
	//设置jb类型
	void setJbType(JBModelType type)
	{
		m_jbType = type;
	}
	//返回jb类型
	JBModelType getJbType()
	{
		return m_jbType;
	}

	//set\get 军标角色：红、蓝
	void           setJBRole(E_PlottingRole role) { m_jbRole = role; }
	E_PlottingRole getJBRole() { return m_jbRole; }

	//设置标绘节点对象Id
	void setNodeobjectId(unsigned int id);

	//返回标绘节点对象id
	unsigned int getNodeobjectId()
	{
		return m_objectid;
	}
	//设置坐标点
	void  setPoints(std::vector<osg::Vec3d> points)
	{
		m_Points = points;
	}
	//设置坐标点
	void setVe2cPoints(std::vector<osg::Vec2>points)
	{
		m_vec2Points = points;
	}
	std::vector<osg::Vec2> getvec2Points()
	{
		return m_vec2Points;
	}
	std::vector<osg::Vec3d> getPoints()
	{
		return m_Points;
	}
	//设置拖动节点
	void setDragNode(osg::ref_ptr<AnnotationEditor> edtir)
	{
		m_anneditr = edtir;
	}
	//返回拖动节点
	osg::ref_ptr<AnnotationEditor> getDragNode()
	{
		return m_anneditr;
	}
	//设置中心点
	void setCenterPoint(osg::Vec3d centerpoint)
	{
		m_CenterPoint = centerpoint;
	}
	//返回中心点
	osg::Vec3d getCenterPoint()
	{
		return  m_CenterPoint;
	}

	void  setImagePath(std::string path)
	{
		m_ImagePath = path;
	}

	std::string getImagePath()
	{
		return m_ImagePath;
	}

	void setDisPoint(osg::Vec3d disppoint)
	{
		m_distancepoint = disppoint;
	}
	osg::Vec3d getDisPoint()
	{
		return m_distancepoint;
	}
	//设置父节点
	void setParentName(QString parentname)
	{
		m_parentName = parentname;
	}
	QString getParentName()
	{
		return m_parentName;
	}
	void setNodeId(QString id)
	{
		m_id = id;
	}
	QString getNodeId()
	{
		return m_id;
	}

	void setPosNode(osg::ref_ptr<osg::MatrixTransform>node)
	{
		m_posNode = node;
	}
	osg::ref_ptr<osg::MatrixTransform>getPosNode()
	{
		return m_posNode;
	}

	////设置标绘节点属性信息
	//void setNodeInfos(NodeInfo info);

	////获取标绘节点属性信息
	//NodeInfo getNodeInfos() {
	//	return m_info;
	//}

	//获取线色
	void getStyleXX(int& r, int& g, int& b, int& a);
	void setStyleXX(int r, int g, int b, int a);

	//获取填充色
	void getStyleTCS(int& r, int& g, int& b, int& a);
	void setStyleTCS(int r, int g, int b, int a);

	//获取线宽
	void getStyleXK(float& xk);
	void setStyleXK(float xk);

	//半径
	void getStyleBJ(float& bj);
	void setStyleBJ(float bj);

	//获取线形
	void getStyleXStyle(osgEarth::Stroke::LineCapStyle& XStyle);
	void setStyleXStyle(osgEarth::Stroke::LineCapStyle XStyle);

#if 1
	//节点名称
	void setTextNode(osg::ref_ptr<osgEarth::PlaceNode> pTextNode) { mp_textNode = pTextNode; }
	osg::ref_ptr<osgEarth::PlaceNode> getTextNode() { return mp_textNode; }
#endif

#if 1
	void setSignageNode(osg::ref_ptr<SignageNode> psignageNode) { mp_signageNode = psignageNode; }
	osg::ref_ptr<SignageNode>getSignageNode() { return mp_signageNode; }
	
#endif 

	QString getModelPath() { return m_sModelPath; }
	void setModelPath(QString sPath) { m_sModelPath = sPath; }

private:
	osgEarth::Style m_NodeStyle;   //节点样式

	QString m_id;

	QString m_name;           //名称

	QString m_parentName;     //父节点名称

	ToolType m_type;             //标绘类型

	unsigned int m_objectid;     //对象id

	std::vector<osg::Vec3d> m_Points;  //坐标点集

	std::vector<osg::Vec2>m_vec2Points;

	osg::Vec3d m_CenterPoint;   //中心点

	osg::ref_ptr<osg::MatrixTransform> m_posNode;

	osg::ref_ptr<AnnotationEditor>  m_anneditr;  //拖动节点

	std::string m_ImagePath;  //图片路径

	osg::Vec3d m_distancepoint;  //圆的外圆点

	JBModelType m_jbType;

	E_PlottingRole m_jbRole;

	//NodeInfo m_info;
	QString m_miaoShu; //m描述

	osg::ref_ptr<osgEarth::PlaceNode> mp_textNode;		//用于显示名称的节点

	osg::ref_ptr<SignageNode> mp_signageNode;           //标牌描述

	QString m_sModelPath; //更换的模型路径

public:
	void getMiaoShu(QString& qstr) { qstr = m_miaoShu; }
	void setMiaoShu(QString qstr) { m_miaoShu = qstr; }
};
