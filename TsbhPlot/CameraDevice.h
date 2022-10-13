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
	//float scope;//����ͷ��ط�Χ
	//float startangle;//����ͷ�����ʼ�Ƕ�
	//float endangle;//����ͷ��ؽ����Ƕ�
	double positionx;
	double positiony;
	osg::Vec3d position;
}S2DModelInfo;

class TsbhPlot_EXPORT CameraDevice
{
public:
	CameraDevice(osg::Group* group, osgEarth::MapNode* node);
	~CameraDevice();
	//�ڶ�ά��ͼ�ﴴ��ģ��
	void buildModel();
	//����ģ�������
	void setPosition(QString id,GeoPoint& gp);
	//����ģ������
	void setModelDatas(QList<S2DModelInfo>values);
    //����2ά��Ӧģ����Ӱ
	void set2DModelMask(QString id,bool visible);
	//����2άģ����Ӱ
	void setModelsVisible(bool visble);
    //ɾ��2D��Ӧģ��
	void removeOne(QString id);

	void removeAll();

	//��ȡģ��ͼ��·��
	std::string getIconPath(int iocnType);

	//��ȡָ��ID��node - add by gkk 20220805
	osg::ref_ptr<osg::Node> getNode(QString nodeID);
private:
	osg::ref_ptr<osg::Group> mp_group;
	//osgEarth::Features::Feature* mFeature;
	//osg::ref_ptr<osgEarth::CircleNode> circle; //��ط�Χ
	//QList<QMap<QString,osg::ref_ptr<osgEarth::CircleNode>>>m_circles;
	//osg::ref_ptr<osgEarth::PlaceNode> mLabel;
	QMap<QString, osg::ref_ptr<osgEarth::PlaceNode>>	m_Labels; //ͼƬ
	QList<S2DModelInfo>									m_modeldatas;
	osg::ref_ptr<osgEarth::MapNode>						mapNode;
	Style pm;
	Style m_circleStyle;
};

