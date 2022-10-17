#pragma once

#include "TsbhTools_global.h"
#include <osg/MatrixTransform>
#include <osg/Referenced>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Vec3>
#include <osgEarth/MapNode>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>

using namespace osgEarth;

class TsbhTools_EXPORT TsbhTools
{
public:
	TsbhTools();
	osg::Group* getRoot();
	virtual void  onLeftButton(GeoPoint* Point) {};

	virtual void  onDoubleClick(GeoPoint* Point) {};

	virtual void  onMouseMove(GeoPoint* Point) {};

	bool m_bFinish = false;
protected:

	int type;
	osg::ref_ptr<osgEarth::MapNode> mapNode;
	osg::ref_ptr<osg::Group> mRoot;
	int pointNum;
};
