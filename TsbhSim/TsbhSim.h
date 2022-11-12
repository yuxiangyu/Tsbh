#pragma once
#include "TsbhSim_global.h"
#include <osgEarth/MapNode>

#include <osg/MatrixTransform>
#include <osg/AnimationPath>

class RTSSCORE_EXPORT TsbhSim
{
public:
	TsbhSim(osgEarth::MapNode* _mapNode);
	~TsbhSim();

	// 加载实体对象，加载失败返回nullptr
	osg::MatrixTransform* LoadEntity(const std::string &strPath, const osg::Matrix &matrix);

	// 创建雷达圆锥图形
	osg::ref_ptr<osg::Geode> BuildRader(float fSpeed, float fRadius, float fHeight);

	// 创建动画
	osg::AnimationPath* CreateAnimationPath(osg::Vec4Array* ctrl, osg::Vec3d scale);

	// 创建彩带
	osg::ref_ptr<osg::Geode> BuildRibbon(int size, osg::MatrixTransform* scaler, float ribbonWidth);

	// 创建飞行尾迹
	osg::MatrixTransform* BuildTail(osg::MatrixTransform* scaler, const osg::Vec3 &pos);

	// 创建飞机历史航迹
	osg::ref_ptr<osg::Group> BuildHistoryRoute(osg::MatrixTransform* scaler, float lineWidth);

private:
	double GetRunTime(osg::Vec3d from, osg::Vec3d to, double speed);
	double GetDis(osg::Vec3d from, osg::Vec3d to);

private:
	osgEarth::MapNode* m_mapNode;
};

