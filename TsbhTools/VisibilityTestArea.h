#pragma once
#include "TsbhTools_global.h"
#include "TsbhTools.h"
#include <osg/MatrixTransform>
#include <osg/Referenced>
#include <osg/NodeCallback>
#include <osg/Vec3>
#include <osgEarth/MapNode>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
#include <osgEarth/PlaceNode>

#include <QVector>
using namespace osgEarth;

namespace osg {
    class PositionAttitudeTransform;
    class TextureCubeMap;
}

class TsbhTools_EXPORT VisibilityTestArea
{
	

public:
	VisibilityTestArea(osg::Group* group,osg::View* view);
	//设置参数
	void setParameter(const osg::Vec3d& Point, double radius);
	~VisibilityTestArea();


	void buildModel();

	
	void updateAttributes();


protected:

   // void generateTestSphere(osg::ref_ptr<osg::TextureCubeMap> depthMap, osg::ref_ptr<osg::TextureCubeMap> colorMap);
    osg::Camera* generateCubeCamera(osg::ref_ptr<osg::TextureCubeMap> cubeMap, unsigned face, osg::Camera::BufferComponent component);

private:
  //  bool _movingMode = false;

	osg::ref_ptr<osg::Group> _shadowedScene;
	osg::ref_ptr<osg::Group> _parentScene;
	osg::ref_ptr<osg::Geode>  _currentDrawNode;

	osg::ref_ptr<osg::Program> _renderProgram;
	osg::ref_ptr<osg::Camera> _depthCameras[6];
  osg::ref_ptr<osg::Camera> _colorCameras[6];
   // osg::Vec3d _pickedPos;
	osg::Vec3d _pickedPos2;

	int _nodeMask;
	int _userRadius;

	osg::View* _view;

};
