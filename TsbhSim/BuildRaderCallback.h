#pragma once

#include <osgViewer/Viewer>
#include <osgEarth/MapNode>
#include <osg/AnimationPath>
#include <osgEarth/Utils>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventHandler>
#include <math.h>

//自定义回调类
class BuildRaderCallback :public osg::NodeCallback
{
public:
	BuildRaderCallback(float fRotateSpeed, float fRotateRadius, float fRotateHeight);
	~BuildRaderCallback();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
private:
	float m_fSpeed; //旋转速度
	float m_fRadius; //距（0，0，0）距离
	float m_fHeight;
};