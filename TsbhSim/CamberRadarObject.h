#ifndef CAMBERRADAROBJECT_H__
#define CAMBERRADAROBJECT_H__

#include <osg/Geometry>
#include "CamberRadarInfo.h"
#include <osg/Geode>
#include <osg/MatrixTransform>
#include "CamberRadarNode.h"
#include <osg/PrimitiveSet>




class CamberRadarObject : public osg::Geode
{
public:
	CamberRadarObject(osg::Group* pGeometryObject, CamberRadarInfo* info,const osgEarth::Color& lineColor, const osgEarth::Color& surfaceColor);
	 ~CamberRadarObject();
	//void SetLineColor(const osgEarth::Color& lineColor);
	//void SetSurfaceColor(osgEarth::Color surfaceColor);

	void SetCamberRadarInfo(const CamberRadarInfo* info);
	const CamberRadarInfo* GetCamberRadarInfo();
private:
	CamberRadarObject();

private:
	void GenerateOuterCurve();
	void GenerateInnerCurve();
	void CreateLeftCrossSection();
	void CreateRightCrossSection();

	void GenerateOuterSurfaceVB();
	void GenerateInnerSurfaceVB();
	void GenerateLeftCrossSectionSurfaceVB();
	void GenerateRightCrossSectionSurfaceVB();
	void GeneratePosition(float _startFovH, float _endFovH, float _startFovV, float _endFovV, int32_t startRadius, int32_t numV,
		const std::vector<std::pair<float, float> >&fovRadiusPairs,  std::vector<osg::Vec3f>& positions);
	void GenerateCrossSectionPosition(float _fovH, float _startFovV, float _endFovV, int32_t startRadius, int32_t numV, std::vector<osg::Vec3f>& positions);
	osg::Geometry::PrimitiveSetList GenerateSurfaceIndex(int32_t numH, int32_t numV);
	osg::Geometry::PrimitiveSetList GenerateLineIndex(int32_t _segmentH, int32_t _segmentV, int32_t _subSegmentH, int32_t _subSegmentV);
	void GenerateOuterFovRadiusPairs(float r);
	void GenerateInnerFovRadiusPairs(float r);
	int32_t FindFovRadiusPairsIndex(float fov, const std::vector<std::pair<float, float> >& fovRadiusPairs);
	float LerpRadius(float fov, const std::vector<std::pair<float, float> >& fovRadiusPairs);
	float Interpolate(float start, float end, float total, float val);
	osg::Vec3f CalPos(float fovH, float fovV);
private:
	osg::ref_ptr<osg::Group> m_pGeometryObject;

    osgEarth::Color m_LineColor;
	osgEarth::Color m_SurfaceColor;
	//osg::ref_ptr<osg::StateSet> m_ptrLineColorSS; // 线颜色
	//osg::ref_ptr<osg::StateSet> m_ptrSurfaceColorSS; // 面颜色
	//osg::ref_ptr<osg::StateSet> m_ptrCullMode;
	osg::ref_ptr<CamberRadarInfo> m_ptrCamberRadarInfo;

	int32_t m_segmentH;
	int32_t m_segmentV;
	int32_t m_subSegmentH;
	int32_t m_subSegmentV;

	osg::ref_ptr<osg::Vec3Array> m_ptrOuterSurfacePositionBuffer;
	osg::ref_ptr<osg::Vec3Array> m_ptrInnerSurfacePositionBuffer;
	osg::ref_ptr<osg::Vec3Array> m_ptrLeftCrossSectionPositionBuffer;
	osg::ref_ptr<osg::Vec3Array> m_ptrRightCrossSectionPositionBuffer;

	osg::Geometry::PrimitiveSetList m_ptrSurfaceIndex;
	osg::Geometry::PrimitiveSetList m_ptrLineIndex;
	osg::Geometry::PrimitiveSetList m_ptrCrossSectionSurfaceIndex;
	osg::Geometry::PrimitiveSetList m_ptrCrossSectionLineIndex;

	osg::ref_ptr<osg::Geometry> m_ptrOuterSurfaceDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrOuterLineDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrInnerSurfaceDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrInnerLineDrawable;

	osg::ref_ptr<osg::Geometry> m_ptrLeftCrossSectionSurfaceDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrLeftCrossSectionLineDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrRightCrossSectionSurfaceDrawable;
	osg::ref_ptr<osg::Geometry> m_ptrRightCrossSectionLineDrawable;

	std::vector<std::pair<float, float> >  m_outerFovRadiusPairs;
	std::vector<std::pair<float, float> >  m_innnerFovRadiusPairs;
};



#endif
