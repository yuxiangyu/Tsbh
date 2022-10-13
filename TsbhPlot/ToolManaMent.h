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
	//��ʼ������¼�
	void StartToolHandler(ToolType type);

	//����������¼�
	void RemoveToolHandler();

	//�����Ƿ������¼�
	void setActive(bool isactive);

	DrawTool* getTool()
	{
		if (m_Tool) {
			return m_Tool.get();
		}
		return NULL;
	}
	//������еı��
	void ClearAllTool();
	//����
	void setAllActive(bool b);
	//��ȡ���ڵ�
	QList<osg::ref_ptr<osg::Node>> getToolNodes(ToolType type);
	QList<osg::ref_ptr<osg::Node>> getAllNodes();

	//��ȡ���ڵ�
	void getToolNode(QString strId, osg::ref_ptr<osg::Node> &Node, osg::ref_ptr<osg::Node> &textNode);

	//��ȡ������ڵ�
	osg::ref_ptr<osg::Node> getToolNode(unsigned int objectid);
	//����
	void setNodeVisible(QString sID, bool bVisible);
	//��λ
	void locationNode(QString sID);
	//ɾ��
	void removeNode(QString sID);

	//������
	osg::ref_ptr<osg::Node>  DrawStaticLine(std::vector<osg::Vec3d> points);

	//����Բ
	osg::ref_ptr<osg::Node> DrawStaticCircle(osg::Vec3d cneterPoint, osg::Vec3d distancepoint);

	//���ƾ�̬����ͼ
	osg::ref_ptr<osg::Node> DrawStaticGeoGatheringPlace(std::vector<osg::Vec2> points);
	//���ƾ���
	osg::ref_ptr<osg::Node> DrawStaticRectangle(std::vector<osg::Vec3d> points);

	//���ƶ����
	osg::ref_ptr<osg::Node> DrawStaticPolygon(std::vector<osg::Vec3d> points);

	//��������
	osg::ref_ptr<osg::Node> DrawStaticWord(osg::Vec3d cneterPoint,QString sText);

	//���Ƶ�λ
	osg::ref_ptr<osg::Node> DrawStaticPos(osg::Vec3d cneterPoint);

	//����ͼƬ
	osg::ref_ptr<osg::Node> DrawStaticPic(osg::Vec3d cneterPoint,std::string path);

	//����jBmodel
	osg::ref_ptr<osg::Node> DrawStaticJBModel(osg::Vec3d cneterPoint, JBModelType type, E_PlottingRole role);

	//���Ʊ������
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name);

	//���Ʊ������
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::Vec3d point, QString name);

	//���ƻ��Ʊ���
	osg::ref_ptr<SignageNode>DrawSignageText(osg::ref_ptr<osg::Node> curNode, QString text = " ");

	//�������κ�����getbound()�����ĵ�ȡ��Ϊ�յ�����
	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec3d> points);

	osg::ref_ptr<osgEarth::PlaceNode> DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec2> points);
	int getNodenum(ToolType type);

	//����ģ��·��
	QString getModelPath() { return m_sModelPath; }
	void setModelPath(QString sPath) { m_sModelPath = sPath; }

	//��ȡ���θ߶�
	double getElevation(double lon, double lat);

	//�������ֱ��text��add by zzf,20220804
	void setText(QString sText) { m_sText = sText; }
	QString getText() { return m_sText; }
private:
	osg::ref_ptr<DrawTool> m_Tool;  //������
	osg::ref_ptr<MapNode>m_MapNode; //��ͼ
	osg::ref_ptr<osg::Group>m_Root; //���ڵ�
	QMap<ToolType, osg::ref_ptr<DrawTool>>m_Tools;
	osg::ref_ptr<osgViewer::View> m_View; //��ͼ�ڵ�

	QString m_sModelPath;

	QString m_sText;//���ֱ��
};

