#include "CamberRadarInfo.h"


CamberRadarInfo::CamberRadarInfo()
:m_startRadius(10000)
, m_radius(30000)
, m_startFovH(0.0)
, m_endFovH(360.0)
, m_startFovV(0)
, m_endFovV(90)
{
}

CamberRadarInfo::~CamberRadarInfo()
{

}

CamberRadarInfo& CamberRadarInfo::operator=(const CamberRadarInfo& rhs)
{
	if (this == &rhs)
		return *this;

	m_startRadius = rhs.m_startRadius;
	m_radius = rhs.m_radius;
	m_startFovH = rhs.m_startFovH;
	m_endFovH = rhs.m_endFovH;
	m_startFovV = rhs.m_startFovV;
	m_endFovV = rhs.m_endFovV;

	m_posRCRS = rhs.m_posRCRS;
	m_srMat = rhs.m_srMat;
	m_modelMat = rhs.m_modelMat;
	return *this;
}

CamberRadarInfo* CamberRadarInfo::Clone() const
{
	CamberRadarInfo* pInfo = new CamberRadarInfo;
	pInfo->m_startRadius = m_startRadius;
	pInfo->m_radius = m_radius;
	pInfo->m_startFovH = m_startFovH;
	pInfo->m_endFovH = m_endFovH;
	pInfo->m_startFovV = m_startFovV;
	pInfo->m_endFovV = m_endFovV;
	pInfo->m_posRCRS = m_posRCRS;
	pInfo->m_srMat = m_srMat;
	pInfo->m_modelMat = m_modelMat;
	return pInfo;
}

