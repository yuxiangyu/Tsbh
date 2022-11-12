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

	// ����ʵ����󣬼���ʧ�ܷ���nullptr
	osg::MatrixTransform* LoadEntity(const std::string &strPath, const osg::Matrix &matrix);

	// �����״�Բ׶ͼ��
	osg::ref_ptr<osg::Geode> BuildRader(float fSpeed, float fRadius, float fHeight);

	// ��������
	osg::AnimationPath* CreateAnimationPath(osg::Vec4Array* ctrl, osg::Vec3d scale);

	// �����ʴ�
	osg::ref_ptr<osg::Geode> BuildRibbon(int size, osg::MatrixTransform* scaler, float ribbonWidth);

	// ��������β��
	osg::MatrixTransform* BuildTail(osg::MatrixTransform* scaler, const osg::Vec3 &pos);

	// �����ɻ���ʷ����
	osg::ref_ptr<osg::Group> BuildHistoryRoute(osg::MatrixTransform* scaler, float lineWidth);

private:
	double GetRunTime(osg::Vec3d from, osg::Vec3d to, double speed);
	double GetDis(osg::Vec3d from, osg::Vec3d to);

private:
	osgEarth::MapNode* m_mapNode;
};

