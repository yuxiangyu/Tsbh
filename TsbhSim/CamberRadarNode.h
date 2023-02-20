#ifndef CAMBERRADARNODE_H__
#define CAMBERRADARNODE_H__

//#include <SDBGeometry/Geometry/Point.h>


#include <osg/Geometry>
#include "CamberRadarNode.h"
#include "CamberRadarInfo.h"
#include <osgEarth/Color>
#include <osgEarth/MapNode>


class RTSSCORE_EXPORT CamberRadarNode : public osg::Group
{

public:
	const CamberRadarInfo* GetCamberRadarInfo();
	/**
	*@brief 设置内曲面半径
	*@param startRadius 内曲面半径
	*/
	void SetStartRadius(float startRadius);
	float GetStartRadius();
	/**
	*@brief 设置外曲面半径
	*@param radius 外曲面半径
	*/
	void SetRadius(float radius);
	float GetRadius();

	/**
	*@brief 设置线条颜色
	*@param lineColor 线条颜色
	*/
	void SetLineColor(osgEarth::Color lineColor);
	osgEarth::Color GetLineColor();

	/**
	*@brief 设置面颜色
	*@param surfaceColor 面颜色
	*/
	void SetSurfaceColor(osgEarth::Color surfaceColor);
	osgEarth::Color GetSurfaceColor();

	/**
	*@brief 设置水平起始角度
	*@param startFovH 水平起始角度
	*/
	void SetStartFovH(float startFovH);
	float GetStartFovH();

	/**
	*@brief 设置水平结束角度
	*@param endFovH 水平结束角度
	*/
	void SetEndFovH(float endFovH);
	float GetEndFovH();

	/**
	*@brief 设置垂直起始角度
	*@param startFovV 垂直起始角度
	*/
	void SetStartFovV(float startFovV);
	float GetStartFovV();

	/**
	*@brief 设置垂直结束角度
	*@param endFovV 垂直结束角度
	*/
	void SetEndFovV(float endFovV);
	float GetEndFovV();

	/**
	*@brief 设置位置
	*@param val 位置
	*/
	void SetPosition(const osgEarth::GeoPoint& val);
	/**
	*@brief 设置方向角，雷达旋转起来相关
	*@param val 方向角
	*/
	void SetHeading(float val);

public:
	CamberRadarNode(osgEarth::MapNode* mapNode);
	 ~CamberRadarNode() {}

   // virtual void Uninitialize();
   // virtual void Update();

	CamberRadarObject* GetRenderObject() const;

	osgEarth::GeoPoint CamberRadarNode::GetCenterPoint();
	osgEarth::MapNode*  getMapNode();
protected:
	osg::ref_ptr<CamberRadarObject> m_ptrRenderObject;
	osg::ref_ptr<CamberRadarInfo> m_ptrCamberRadarInfo;
	osgEarth::Color m_LineColor;
	osgEarth::Color m_SurfaceColor;
	osg::ref_ptr<osgEarth::MapNode > _mapNode;
};




#endif