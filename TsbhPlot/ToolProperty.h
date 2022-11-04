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
//	std::string strId;            //���id
//	std::string name;             //�������
//	osg::ref_ptr<osg::Node> node; //���ڵ�
//	unsigned int objectid;         //����id
//	osg::ref_ptr<AnnotationEditor>  anneditr; //��涯̬��
//	std::vector<osg::Vec3d> points;  //�����
//	std::string miaoShu;
//	bool biaotou;
//};

enum ToolType
{
	LINE,			// ��
	IMAGE,			//ͼƬ
	CIRCLE,			// Ȧ
	TEXT,			// ����
	POLYGON,		// �����
	RECTANGLE,		// ����
	MARK,			// ����
	GEOGETHER,		//����ͼ
	GEOPOSITION,	//�����λ
	RUNWAY,
};

enum JBModelType
{
	//!!!ע��!!!�����ı������һ��Ҫ����ţ����� NOTYPE ǰһ��
	ZXKMODEL,   //���Ŀ�
	DJKMODEL,  //������
	FSCDMODEL, //�������
	ZBYCMODEL, //ս������
	DJQYMODEL, //��������
	YQMODEL,   //Ӫ��
	GFJWMODEL, //��������
	GLSKMODEL,
	TXGLMODEL,  //ͨ�Ź���
	DXDDZXK,      //���µ������Ŀ�
	ZHC,        //ָ�ӳ�
	DDFSZD,     //�����������
	ZCC,       //��쳵
	DXDDZXC,   //���µ������Ŀ�
	ZYZD,      //ת�����
	DSGDTXSN,  //���Ϲ̶�ͨ����Ŧ
	DDFSC,     //�������䳵
	GCZBBZC,   //����װ�����ϳ�
	TXC,       //ͨ�ų�
	QC,        //����
	TZC,       //��װ��
	LJYCDKQBLD, //½��Զ�̶Կ��鱨�״�
	LDQBZ,      //�״��鱨վ
	FKBQZD,     //���ձ�Ⱥ���
	YJJ,        //Ԥ����
	HZJ,        //��ը��
	ZDJ,        //ս���� 
	HKMJ,       //����ĸ��
	JB_HQBZBD,	//���ڱ��ϲ����֣��ӡ�Ⱥ
	JB_ZBBZBD,	//װ�����ϲ����֣��ӡ�Ⱥ
	JB_YLJHZ,	//ҽ�ƾȻ���

	//һ��Ҫ�������
	NOTYPE,		//�����ͣ������ж��Ƿ����ڽ��б�Ż���
};

//����ɫ-�졢�� - add by gkk 20220516
enum class E_PlottingRole : int {
	RED = 0,			//��ʾ�췽
	BLUE				//��ʾ����
};

class TsbhPlot_EXPORT ToolProperty : public osg::Referenced
{
public:
	ToolProperty();
	~ToolProperty();
	//���ñ����ʽ����
	void setNodeStyle(osgEarth::Style style);
	//��ȡ�����ʽ����
	osgEarth::Style getNodeStyle() {
		return m_NodeStyle;
	}
	//��������
	void setToolName(QString name);
	//��ȡ����
	QString getToolName() {
		return m_name;
	}
	//���ñ������
	void setToolType(ToolType type);

	//��ȡ��ʽ����
	ToolType getToolType() {
		return m_type;
	}
	//����jb����
	void setJbType(JBModelType type)
	{
		m_jbType = type;
	}
	//����jb����
	JBModelType getJbType()
	{
		return m_jbType;
	}

	//set\get �����ɫ���졢��
	void           setJBRole(E_PlottingRole role) { m_jbRole = role; }
	E_PlottingRole getJBRole() { return m_jbRole; }

	//���ñ��ڵ����Id
	void setNodeobjectId(unsigned int id);

	//���ر��ڵ����id
	unsigned int getNodeobjectId()
	{
		return m_objectid;
	}
	//���������
	void  setPoints(std::vector<osg::Vec3d> points)
	{
		m_Points = points;
	}
	//���������
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
	//�����϶��ڵ�
	void setDragNode(osg::ref_ptr<AnnotationEditor> edtir)
	{
		m_anneditr = edtir;
	}
	//�����϶��ڵ�
	osg::ref_ptr<AnnotationEditor> getDragNode()
	{
		return m_anneditr;
	}
	//�������ĵ�
	void setCenterPoint(osg::Vec3d centerpoint)
	{
		m_CenterPoint = centerpoint;
	}
	//�������ĵ�
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
	//���ø��ڵ�
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

	////���ñ��ڵ�������Ϣ
	//void setNodeInfos(NodeInfo info);

	////��ȡ���ڵ�������Ϣ
	//NodeInfo getNodeInfos() {
	//	return m_info;
	//}

	//��ȡ��ɫ
	void getStyleXX(int& r, int& g, int& b, int& a);
	void setStyleXX(int r, int g, int b, int a);

	//��ȡ���ɫ
	void getStyleTCS(int& r, int& g, int& b, int& a);
	void setStyleTCS(int r, int g, int b, int a);

	//��ȡ�߿�
	void getStyleXK(float& xk);
	void setStyleXK(float xk);

	//�뾶
	void getStyleBJ(float& bj);
	void setStyleBJ(float bj);

	//��ȡ����
	void getStyleXStyle(osgEarth::Stroke::LineCapStyle& XStyle);
	void setStyleXStyle(osgEarth::Stroke::LineCapStyle XStyle);

#if 1
	//�ڵ�����
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
	osgEarth::Style m_NodeStyle;   //�ڵ���ʽ

	QString m_id;

	QString m_name;           //����

	QString m_parentName;     //���ڵ�����

	ToolType m_type;             //�������

	unsigned int m_objectid;     //����id

	std::vector<osg::Vec3d> m_Points;  //����㼯

	std::vector<osg::Vec2>m_vec2Points;

	osg::Vec3d m_CenterPoint;   //���ĵ�

	osg::ref_ptr<osg::MatrixTransform> m_posNode;

	osg::ref_ptr<AnnotationEditor>  m_anneditr;  //�϶��ڵ�

	std::string m_ImagePath;  //ͼƬ·��

	osg::Vec3d m_distancepoint;  //Բ����Բ��

	JBModelType m_jbType;

	E_PlottingRole m_jbRole;

	//NodeInfo m_info;
	QString m_miaoShu; //m����

	osg::ref_ptr<osgEarth::PlaceNode> mp_textNode;		//������ʾ���ƵĽڵ�

	osg::ref_ptr<SignageNode> mp_signageNode;           //��������

	QString m_sModelPath; //������ģ��·��

public:
	void getMiaoShu(QString& qstr) { qstr = m_miaoShu; }
	void setMiaoShu(QString qstr) { m_miaoShu = qstr; }
};
