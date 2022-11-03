#pragma once

#include <osgViewer/Viewer>
#include <osgEarth/MapNode>
#include <osg/AnimationPath>
#include <osgEarth/Utils>
#include <osgParticle/FireEffect>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventHandler>
#include <math.h>
#include <iostream>
#include <fstream>
#include <osg/LineWidth>
#include "TsbhSim_global.h"

//�Զ������ص���
class RTSSCORE_EXPORT CreateTrackCallback :public osg::NodeCallback
{
public:
	CreateTrackCallback(osg::Group* root, float ribbonWidth);
	~CreateTrackCallback();

	osg::ref_ptr<osg::Geode> BuildTrack(osg::Vec3 m_Vec3LatPoint, osg::Vec3 m_Vec3CurPoint);
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	//��һ֡ģ��λ�������
	osg::Vec3 m_Vec3LastPosition;
	//��ǰģ��λ�������
	osg::Vec3 m_Vec3CurPosition;
	osg::observer_ptr<osg::Geometry> m_opGeometryRibbon;
	osg::Group* m_proot;
	float m_fwidth;
};