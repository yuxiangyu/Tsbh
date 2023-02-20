#include "CamberRadarNode.h"
#include "CamberRadarObject.h"
#include <osgEarth/GeoTransform>

CamberRadarNode::CamberRadarNode(osgEarth::MapNode* mapNode)
{
	m_ptrCamberRadarInfo = new CamberRadarInfo;
	m_ptrRenderObject = new CamberRadarObject(this);
	_mapNode = mapNode;
	//SetVisibleMask(15);
	//SetIntersectMask(0);
	//m_NodeDirty.reset();
	//SetMaxViewDistance(10000000);

	//k3SDG::Point pt;
	//CRSUtil::Instance()->GetSDBPointP(pt, k3CU::Vec3d(0, 0, 0));
	//SetSDBGeometry(&pt);
	//InitCRS(&pt);

}

CamberRadarObject* CamberRadarNode::GetRenderObject() const
{
	return m_ptrRenderObject;
}

void CamberRadarNode::SetStartRadius(float startRadius)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_startRadius = startRadius;
//	m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetRadius(float radius)
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_radius = radius;
	//m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetLineColor(osgEarth::Color lineColor)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	m_LineColor = lineColor;
	m_ptrRenderObject->SetLineColor(lineColor);
}

void CamberRadarNode::SetSurfaceColor(osgEarth::Color surfaceColor)
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	m_SurfaceColor = surfaceColor;
	m_ptrRenderObject->SetSurfaceColor(surfaceColor);
}

void CamberRadarNode::SetStartFovH(float startFovH)
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_startFovH = startFovH;
	//m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetEndFovH(float endFovH)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_endFovH = endFovH;
	//m_NodeDirty.set(DM_Geometry);
}

float CamberRadarNode::GetEndFovH()
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_endFovH;
}

void CamberRadarNode::SetStartFovV(float startFovV)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_startFovV = startFovV;
	//m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetEndFovV(float endFovV)
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	m_ptrCamberRadarInfo->m_endFovV = endFovV;
	//m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetPosition(const osgEarth::GeoPoint& val)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	//k3CU::Vec3d posRCRS = val;
	//ReprojectionF2R(posRCRS);
	m_ptrCamberRadarInfo->m_posRCRS = val;

	osg::ref_ptr<osgEarth::GeoTransform> xform = new osgEarth::GeoTransform;
	xform->setPosition(val);
	m_ptrCamberRadarInfo->m_srMat = xform->getMatrix();
	//m_NodeDirty.set(DM_Geometry);
}

void CamberRadarNode::SetHeading(float val)
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	//k3CU::Quat postureQuat;
	//k3RE::EularQuatConverter::Eular2QuatDegreePlanar(val, 0, 0, postureQuat);
//	k3CU::Matrix matrix(postureQuat);
	osg::Quat quat;
	quat.makeRotate(osg::inDegrees(val), osg::Vec3d(0, 0, 1));

	m_ptrCamberRadarInfo->m_modelMat = osg::Matrix::rotate(quat);
//	m_NodeDirty.set(DM_Geometry);
}



CamberRadarObject* CamberRadarNode::GetRenderObject() const
{
	return m_ptrRenderObject.get();
}



float CamberRadarNode::GetStartRadius()
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_startRadius;
}

float CamberRadarNode::GetRadius()
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_radius;
}

osgEarth::Color CamberRadarNode::GetLineColor()
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	return m_LineColor;
}

osgEarth::Color CamberRadarNode::GetSurfaceColor()
{
//	k3CU::AutoMutex autoMutex(m_Mutex);
	return m_SurfaceColor;
}

float CamberRadarNode::GetStartFovH()
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_startFovH;
}

float CamberRadarNode::GetStartFovV()
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_startFovV;
}

float CamberRadarNode::GetEndFovV()
{
	//k3CU::AutoMutex autoMutex(m_Mutex);
	return m_ptrCamberRadarInfo->m_endFovV;
}

const CamberRadarInfo* CamberRadarNode::GetCamberRadarInfo()
{
	return m_ptrCamberRadarInfo;
}


osgEarth::GeoPoint CamberRadarNode::CamberRadarNode::GetCenterPoint()
{
	return m_ptrCamberRadarInfo->m_posRCRS;
}

osgEarth::MapNode*  CamberRadarNode::getMapNode()
{
	return _mapNode;
}

