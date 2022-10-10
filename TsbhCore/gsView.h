#pragma once

#include "rtsscore_global.h"

#include <QtCore/qglobal.h>
#include <QtOpenGL/QGLWidget>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>


#include <osgEarth/ArcGISServer>
/// paths to models
//#include <simUtil/ExampleResources.h>

#include <osgEarth/StringUtils>
#include <osgEarth/Style>
#include <osgEarth/LatLongFormatter>
#include <osgEarth/MGRSFormatter>

#include <QtWidgets>
#include <QTimer>
#include "zMapEntity.h"
#include "zlayer.h"
#include <osgEarth/EarthManipulator>
#include <osgEarth/MapNode>
#include <osgViewer/View>

using namespace osg;

/*************************************
* Brief     : ���ڶ���ά���泡���ӽ�ͬ��
*************************************/
class RTSSCORE_EXPORT Darg2Dto3DHandle :public osgGA::GUIEventHandler
{
public:
	Darg2Dto3DHandle();
	~Darg2Dto3DHandle();
	Darg2Dto3DHandle(osgEarth::EarthManipulator* view);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:
	osgEarth::EarthManipulator* view;
	bool active;
};

/*************************************
* Brief     : ���泡����ͼ
*************************************/
class RTSSCORE_EXPORT gsView: public QWidget
{
	Q_OBJECT

public:
	//gsView(bool show3D = false);
	gsView();
	virtual ~gsView();

	//������ά�������� - add by gkk 20220623
	void config3DMap();
	void configMaps(zMapEntity* mapentity);
	void updateLayer(zLayer*);
	void addLodGroup();
	//����ͼ��
	void addLayer(zLayer*);
	void removeLayer(zLayer*);
	void removeAllLayers(osgEarth::MapNode* scenMan);
	void loadSceneModel(string modelpath,string name, osg::Vec3d &p, osg::Vec3d &scale, osg::Vec3d &rph);
	void removeSceneModel(string name);

	osgViewer::View* get3DScene() { return viewer3d; }
	osgViewer::View* get2DScene() { return viewer2d; }
	osg::Group* get3dGroup(){ return targetGroup_3D; }
	osg::Group* get2dGroup() { return group2d; }
	osg::LOD* getLod() { return lod; }
	osg::LOD *get2DLod() { return lod_2d; }
	osg::Matrix& getModelMatrix(string name);
	osgEarth::EarthManipulator* get2dManipulator(){ return mainip2d; }

	void set2DViewpoint(osgEarth::Viewpoint vp);
	void clearGroudObjects();

	//�ڶ�����ά���������躯�� - add by gkk 20220922
	bool has3DScene_2();
	osgViewer::View* get3DScene_2();
	osg::Group* get3dGroup_2() { return targetGroup_3D_2; }
	osg::LOD* getLod_2() { return lod_3d_2; }


private:
	//��ʼ����ά��ͼ-�󳡾�
	void initEarth();

	//��ʼ����ά��ͼ-����С����
	void initEarth2();

	//��ʼ����ά��ͼ
	void make2dMap();

private:
	//3D�������ڵ�
	osg::Group* targetGroup_3D;

	//2D�������ڵ�
	osg::Group* group2d;

	
	//osg::Group* targetGroup_3D2;
	//����Ŀ��չʾ����
	//vector<AnnotationNode*> GroudObjectsShown2d,GroudObjectsShown3d;
	//����ʵ��
	
	Group* group;
	osg::ref_ptr<osgViewer::View> viewer3d;
	osg::ref_ptr<osgEarth::MapNode> viewer3dNode;
	osg::ref_ptr<osgViewer::View> viewer2d;
	osg::ref_ptr<osgEarth::MapNode> viewer2dNode;
	osgEarth::EarthManipulator* mainip3d;
	osgEarth::EarthManipulator* mainip2d;
	//osg::ref_ptr<simVis::ViewManager>   viewMan_3d;
	//osg::ref_ptr<simVis::SceneManager>  sceneMan_3d;
	//osg::ref_ptr<simVis::ViewManager>   viewMan_2d;
	//osg::ref_ptr<simVis::SceneManager>  sceneMan_2d;

	//��ά���������
	osg::Matrix mCameraOffset2D; 

	//
	bool satCameraExist;
	//���� ���ǵ������
	//osgEarth::EarthManipulator* mainip2d;
	osg::LOD* lod;
	osg::LOD* lod_2d;
	//osg::Group* ggive = nullptr;
	//osg::Group* ggqx = nullptr;
    //osg::MatrixTransform* pat= nullptr;

	//�ڶ�����ά������������� - add by gkk 20220922
	osg::Group*							targetGroup_3D_2;
	osg::ref_ptr<osgViewer::View>			viewer3d_2 = nullptr;
	osgEarth::EarthManipulator*			mainip3d_2;
	//osg::ref_ptr<simVis::ViewManager>   viewMan_3d_2;
	//osg::ref_ptr<simVis::SceneManager>  sceneMan_3d_2;
	osg::LOD*							lod_3d_2;
};

