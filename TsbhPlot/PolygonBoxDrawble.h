#pragma once
#include <osgEarth/BBoxSymbol>
#include <osgEarth/Common>
#include "TsbhPlot_global.h"
#include <osg/Geometry>
#include <osg/BoundingBox>
class TsbhPlot_EXPORT SimpleBoxDrawable :public osg::Geometry
{
public:
	SimpleBoxDrawable(const osg::BoundingBox& box,int type, const osgEarth::BBoxSymbol& boxSymbol);
	SimpleBoxDrawable(const osg::BoundingBox& box,int type, float margin=2.0f);
	SimpleBoxDrawable(const osg::BoundingBox& box,int type, float marginTop, float marginBottom , float marginLeft , float marginRight);
protected:
	SimpleBoxDrawable() { }
	SimpleBoxDrawable(const SimpleBoxDrawable& rhs, const osg::CopyOp& op) { }

};
class TsbhPlot_EXPORT PolygonBoxDrawable:public osg::Geometry
{
public:
	
	PolygonBoxDrawable(const osg::BoundingBox& box, const osgEarth::BBoxSymbol& boxSymbol);
	PolygonBoxDrawable(const osg::BoundingBox& box, float margin = 2.0f);
	PolygonBoxDrawable(const osg::BoundingBox & box, float marginTop, float marginBottom, float marginLeft, float marginRight);
protected:
	PolygonBoxDrawable() { }
	PolygonBoxDrawable(const PolygonBoxDrawable& rhs, const osg::CopyOp& op) { }
private:
	float corner;
	float top;
	float left;
};
class TsbhPlot_EXPORT ComplexBoxDrawable :public osg::Geometry
{
public:

	ComplexBoxDrawable(const osg::BoundingBox& box, const osgEarth::BBoxSymbol& boxSymbol);
	ComplexBoxDrawable(const osg::BoundingBox& box, float margin = 2.0f);
	ComplexBoxDrawable(const osg::BoundingBox & box, float marginTop, float marginBottom, float marginLeft, float marginRight);
protected:
	ComplexBoxDrawable() { }
	ComplexBoxDrawable(const ComplexBoxDrawable& rhs, const osg::CopyOp& op) { }
private:
	float top;
	float left;
};
