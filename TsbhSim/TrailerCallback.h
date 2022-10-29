#pragma once

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventHandler>
#include <math.h>

// ·ÉÐÐ²Ê´ø
class TrailerCallback :public osg::NodeCallback
{
public:
	TrailerCallback(osg::Geometry* ribbon, int size, int ribbonWidth);

	~TrailerCallback();

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
	osg::observer_ptr<osg::Geometry> m_opGeometryRibbon;
	int m_nsize;
	int m_nwidth;
};

