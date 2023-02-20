#ifndef CAMBERRADARINFO_H_
#define CAMBERRADARINFO_H_

#include <osgEarth/Color>

#include <vector>
#include <osg/Matrix>

#include "TsbhSim_global.h"
#include <osgEarth/GeoPositionNode>



class RTSSCORE_EXPORT CamberRadarInfo
{
public:
	CamberRadarInfo();
	virtual ~CamberRadarInfo();

	CamberRadarInfo& operator = (const CamberRadarInfo& rhs);

public:
	CamberRadarInfo* Clone() const;

public:
	float m_startRadius;         ///< 内曲面半径
	float m_radius;             ///< 外曲面半径
	float m_startFovH;             ///< 左横截面角度
	float m_endFovH;             ///< 右横截面角度
	float m_startFovV;             ///< 垂直起始角度
	float m_endFovV;             ///< 垂直结束角度

	osgEarth::GeoPoint m_posRCRS;      ///< 位置
	osg::Matrix m_srMat;
	osg::Matrix m_modelMat;

};




#endif
