#pragma once

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventHandler>
#include <math.h>
#include <osgParticle/FireEffect>
#include "TsbhSim_global.h"

// ·ÉÐÐ²Ê´ø
class RTSSCORE_EXPORT TrailerCallback :public osg::NodeCallback
{
public:
	TrailerCallback(osg::Geometry* ribbon, int size, float ribbonWidth);
	TrailerCallback(osgParticle::FireEffect* fire);
	~TrailerCallback();

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
	osg::observer_ptr<osg::Geometry> m_opGeometryRibbon = nullptr;
	osg::ref_ptr<osgParticle::FireEffect> m_rpFire = nullptr;
	int m_nsize;
	float m_fwidth;
};

