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
#include "TsbhSim_global.h"

//�Զ���ص���
class RTSSCORE_EXPORT BuildRaderCallback :public osg::NodeCallback
{
public:
	BuildRaderCallback(float fRotateSpeed, float fRotateRadius, float fRotateHeight);
	~BuildRaderCallback();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
private:
	float m_fSpeed; //��ת�ٶ�
	float m_fRadius; //�ࣨ0��0��0������
	float m_fHeight;
};