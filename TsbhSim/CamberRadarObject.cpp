#include "CamberRadarObject.h"





CamberRadarObject::CamberRadarObject(osg::Group* pGeometryObject, CamberRadarInfo* info, const osgEarth::Color& lineColor, const osgEarth::Color& surfaceColor)
: m_pGeometryObject(pGeometryObject)
, m_ptrCamberRadarInfo(info)
, m_LineColor(lineColor)
, m_SurfaceColor(surfaceColor)
{
	pGeometryObject->addChild(this);
	//SetLineColor(m_LineColor);
	//SetSurfaceColor(m_SurfaceColor);

	m_segmentH = 20;
	m_segmentV = 20;
	m_subSegmentH = 3;
	m_subSegmentV = 3;

	//m_ptrCullMode = k3RE::StateSet::CreateInstance();
	//m_ptrCullMode->SetAttribute(RERoot()->GetStateAttributeManager()->GetPublicStateAttribute(k3RE::PSA_CullMode_AntiClockWise), k3RE::StateAttribute::OAndP);

	int32_t startRadius = m_segmentH * m_subSegmentH;
	int32_t num = m_segmentV * m_subSegmentV;
	m_ptrSurfaceIndex = GenerateSurfaceIndex(startRadius, num);
	m_ptrLineIndex = GenerateLineIndex(m_segmentH, m_segmentV, m_subSegmentH, m_subSegmentV);

	GenerateOuterFovRadiusPairs(m_ptrCamberRadarInfo->m_radius);
	GenerateOuterSurfaceVB();
	GenerateOuterCurve();

	GenerateInnerFovRadiusPairs(m_ptrCamberRadarInfo->m_startRadius);
	GenerateInnerSurfaceVB();
	GenerateInnerCurve();

	CreateLeftCrossSection();
	CreateRightCrossSection();
}

CamberRadarObject::~CamberRadarObject()
{

}


void CamberRadarObject::SetCamberRadarInfo(const CamberRadarInfo* info)
{
	int32_t startRadius = m_segmentH * m_subSegmentH;
	int32_t numV = m_segmentV * m_subSegmentV;

	if (m_ptrCamberRadarInfo->m_startFovH!=info->m_startFovH || m_ptrCamberRadarInfo->m_endFovH!=info->m_endFovH ||
		m_ptrCamberRadarInfo->m_startFovV!=info->m_startFovV || m_ptrCamberRadarInfo->m_endFovV!=info->m_endFovV ||
		m_ptrCamberRadarInfo->m_radius!=info->m_radius)
	{
		if (m_ptrCamberRadarInfo->m_radius!=info->m_radius)
		{
			GenerateOuterFovRadiusPairs(info->m_radius);
		}

		std::vector<osg::Vec3f> positions;
		GeneratePosition(osg::DegreesToRadians(info->m_startFovH), osg::DegreesToRadians(info->m_endFovH), osg::DegreesToRadians(info->m_startFovV),
			osg::DegreesToRadians(info->m_endFovV), startRadius, numV, m_outerFovRadiusPairs, positions);
		if (!m_ptrOuterSurfacePositionBuffer.valid())
			m_ptrOuterSurfacePositionBuffer = new osg::Vec3Array();
		else
			m_ptrOuterSurfacePositionBuffer->clear();
		for (int i = 0; i < positions.size(); i++)
			m_ptrOuterSurfacePositionBuffer->push_back(positions[i]);
	}

	if (m_ptrCamberRadarInfo->m_startFovH!=info->m_startFovH || m_ptrCamberRadarInfo->m_endFovH!=info->m_endFovH ||
		m_ptrCamberRadarInfo->m_startFovV!= info->m_startFovV || m_ptrCamberRadarInfo->m_endFovV!= info->m_endFovV ||
		m_ptrCamberRadarInfo->m_startRadius!= info->m_startRadius)
	{
		if (m_ptrCamberRadarInfo->m_startRadius!= info->m_startRadius)
		{
			GenerateInnerFovRadiusPairs(info->m_startRadius);
		}

		std::vector<osg::Vec3f> positions;
		GeneratePosition(osg::DegreesToRadians(info->m_startFovH), osg::DegreesToRadians(info->m_endFovH), osg::DegreesToRadians(info->m_startFovV),
			osg::DegreesToRadians(info->m_endFovV), startRadius, numV, m_innnerFovRadiusPairs, positions);
		//osg::Vec3f* pData = static_cast<k3CU::Vec3f*>(m_ptrInnerSurfacePositionBuffer->Lock(false));

		//memcpy(pData, &positions[0], positions.size()*sizeof(k3CU::Vec3f));
		//m_ptrInnerSurfacePositionBuffer->Unlock();
		if (!m_ptrInnerSurfacePositionBuffer.valid())
			m_ptrInnerSurfacePositionBuffer = new osg::Vec3Array();
		else
			m_ptrInnerSurfacePositionBuffer->clear();
		for (int i = 0; i < positions.size(); i++)
			m_ptrInnerSurfacePositionBuffer->push_back(positions[i]);
	}

	if (m_ptrCamberRadarInfo->m_startFovH!= info->m_startFovH || m_ptrCamberRadarInfo->m_endFovH!=info->m_endFovH ||
		m_ptrCamberRadarInfo->m_startFovV!= info->m_startFovV || m_ptrCamberRadarInfo->m_endFovV!= info->m_endFovV||
		m_ptrCamberRadarInfo->m_radius!= info->m_radius || m_ptrCamberRadarInfo->m_startRadius!=info->m_startRadius)
	{
		if (m_ptrCamberRadarInfo->m_radius!=info->m_radius)
		{
			GenerateOuterFovRadiusPairs(info->m_radius);
		}

		if (m_ptrCamberRadarInfo->m_startRadius!=info->m_startRadius)
		{
			GenerateInnerFovRadiusPairs(info->m_startRadius);
		}

		std::vector<osg::Vec3f> positions;
		GenerateCrossSectionPosition(osg::DegreesToRadians(info->m_startFovH), osg::DegreesToRadians(info->m_startFovV),
			osg::DegreesToRadians(info->m_endFovV), 10, numV,positions);
	//	k3CU::Vec3f* pLeftCrossSectionData = static_cast<k3CU::Vec3f*>(m_ptrLeftCrossSectionPositionBuffer->Lock(false));

		//memcpy(pLeftCrossSectionData, &positions[0], positions.size()*sizeof(k3CU::Vec3f));
		//m_ptrLeftCrossSectionPositionBuffer->Unlock();

		if (!m_ptrLeftCrossSectionPositionBuffer.valid())
			m_ptrLeftCrossSectionPositionBuffer = new osg::Vec3Array();
		else
			m_ptrLeftCrossSectionPositionBuffer->clear();
		for (int i = 0; i < positions.size(); i++)
			m_ptrLeftCrossSectionPositionBuffer->push_back(positions[i]);

		GenerateCrossSectionPosition(osg::DegreesToRadians(info->m_endFovH), osg::DegreesToRadians(info->m_startFovV),
			osg::DegreesToRadians(info->m_endFovV), 10, numV, positions);
		//k3CU::Vec3f* pRightCrossSectionData = static_cast<k3CU::Vec3f*>(m_ptrRightCrossSectionPositionBuffer->Lock(false));

		//memcpy(pRightCrossSectionData, &positions[0], positions.size()*sizeof(k3CU::Vec3f));
		//m_ptrRightCrossSectionPositionBuffer->Unlock();
		if (!m_ptrRightCrossSectionPositionBuffer.valid())
			m_ptrRightCrossSectionPositionBuffer = new osg::Vec3Array();
		else
			m_ptrRightCrossSectionPositionBuffer->clear();
		for (int i = 0; i < positions.size(); i++)
			m_ptrRightCrossSectionPositionBuffer->push_back(positions[i]);
	}

//	m_BoundingSphere.init();
	//m_bBSComputed = true;
	//m_BoundingSphere._center = info->m_posRCRS;
	//m_BoundingSphere._radius = info->m_radius;
	m_ptrCamberRadarInfo = info->Clone();
}

const CamberRadarInfo* CamberRadarObject::GetCamberRadarInfo()
{
	return m_ptrCamberRadarInfo;
}

void CamberRadarObject::GenerateOuterCurve()
{
	//surface
	m_ptrOuterSurfaceDrawable = new osg::Geometry;
	//m_ptrOuterSurfaceDrawable->SetJoinInSelect(false);
	if (m_ptrSurfaceIndex.size()==0)
	{
		int32_t startRadius = m_segmentH * m_subSegmentH;
		int32_t num = m_segmentV * m_subSegmentV;
		m_ptrSurfaceIndex = GenerateSurfaceIndex(startRadius, num);
	}

	m_ptrOuterSurfaceDrawable->setPrimitiveSetList(m_ptrSurfaceIndex);

	if (!m_ptrOuterSurfacePositionBuffer.valid())
	{
		GenerateOuterSurfaceVB();
	}

	m_ptrOuterSurfaceDrawable->setVertexArray(m_ptrOuterSurfacePositionBuffer);

	
	//k3RE::DrawArraysPtr ptrSurfaceDrawArrays = new k3RE::DrawArrays(k3RS::OT_Triangle_List,
	//	0, m_ptrSurfaceIndex->GetIndexNumber(), 0, m_ptrOuterSurfacePositionBuffer->GetVertexNum(), true);

	//m_ptrOuterSurfaceDrawable->SetDrawArrays(ptrSurfaceDrawArrays);	

	//line
	m_ptrOuterLineDrawable = new osg::Geometry;
	//m_ptrOuterLineDrawable->SetJoinInSelect(false);
	//zhushi111111
	/*if (m_ptrLineIndex.size()==0))
	{
		m_ptrLineIndex = GenerateLineIndex(m_segmentH, m_segmentV, m_subSegmentH, m_subSegmentV);
	}

	m_ptrOuterLineDrawable->SetIndexBuffer(m_ptrLineIndex);
	m_ptrOuterLineDrawable->SetPositionBuffer(m_ptrOuterSurfacePositionBuffer);


	k3RE::DrawArraysPtr ptrLineDrawArrays = new k3RE::DrawArrays(k3RS::OT_Line_List,
		0, m_ptrLineIndex->GetIndexNumber(), 0, m_ptrOuterSurfacePositionBuffer->GetVertexNum(), true);

	m_ptrOuterLineDrawable->SetDrawArrays(ptrLineDrawArrays);*/
	//zhushi111111
}

osg::Geometry::PrimitiveSetList CamberRadarObject::GenerateSurfaceIndex(int32_t numH, int32_t numV)
{
	osg::Geometry::PrimitiveSetList ptrLineIndex;
//	int32_t indexNum = numH * numV * 4;
	//k3CU::MemBufferPtr ptrIBPos = k3CU::MemBuffer::Create(indexNum*sizeof(uint16_t));
	//uint16_t* pIBData = static_cast<uint16_t*>(ptrIBPos->GetBuffer());
	for (int32_t i = 0; i < numH; ++i)
	{
		osg::ref_ptr<osg::DrawElementsUInt> quad =
			new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
		for (int32_t j = 0; j < numV; ++j)
		{
			int32_t value1 = j * (numH + 1) + i;
			int32_t	value2 = j * (numH + 1) + i + 1;
			int32_t	value3 = (j + 1) * (numH + 1) + i;
			int32_t	value4 = (j + 1) * (numH + 1) + i + 1;
			
			quad->push_back(value1);
			quad->push_back(value2);
			quad->push_back(value3);
			quad->push_back(value4);
		}
		
		//new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4)
		ptrLineIndex.push_back(quad);
	}
	//ptrLineIndex = k3RE::IndexBuffer::Create(k3RE::IndexBuffer::IBTS_INT16,
	//	k3RE::IndexBuffer::DEFAULT, indexNum, ptrIBPos);
	return ptrLineIndex;
}

k3RE::IndexBufferPtr CamberRadarObject::GenerateLineIndex(int32_t _segmentH, int32_t _segmentV, int32_t _subSegmentH, int32_t _subSegmentV)
{
	k3RE::IndexBufferPtr ptrLineIndex;
	int32_t startRadius = _segmentH * _subSegmentH;
	int32_t num = _segmentV * _subSegmentV;
	int32_t indexNum = (_segmentH + 1) * (2 * num) + (_segmentV + 1) * (2 * startRadius);
	k3CU::MemBufferPtr ptrIBPos = k3CU::MemBuffer::Create(indexNum*sizeof(uint16_t));
	uint16_t* pIBData = static_cast<uint16_t*>(ptrIBPos->GetBuffer());
	for (int32_t i = 0; i < _segmentH + 1; ++i)
		for (int32_t j = 0; j < num; ++j) 
		{
			int32_t segmentH = i * _subSegmentH;
			pIBData[2 * (i * num + j)] = j * (startRadius + 1) + segmentH;
			pIBData[2 * (i * num + j) + 1] = (j + 1) * (startRadius + 1) + segmentH;
		}

	int32_t offsetIndex = (_segmentH + 1) * (2 * num);
	for (int32_t i = 0; i < _segmentV + 1; ++i)
		for (int32_t j = 0; j < startRadius; ++j) 
		{
			int32_t segmentV = i * _subSegmentV;
			pIBData[offsetIndex + 2 * (j + i * startRadius)] = segmentV * (startRadius + 1) + j;
			pIBData[offsetIndex + 2 * (j + i * startRadius) + 1] = segmentV * (startRadius + 1) + j + 1;
		}

	ptrLineIndex = k3RE::IndexBuffer::Create(k3RE::IndexBuffer::IBTS_INT16,
		k3RE::IndexBuffer::DEFAULT, indexNum, ptrIBPos);
	return ptrLineIndex;
}

void CamberRadarObject::GenerateOuterSurfaceVB()
{
	int32_t startRadius = m_segmentH * m_subSegmentH;
	int32_t numV = m_segmentV * m_subSegmentV;

	int32_t pointNum = (startRadius + 1) * (numV + 1);

	//k3CU::MemBufferPtr ptrMBPos = k3CU::MemBuffer::Create(pointNum*sizeof(k3CU::Vec3f));
	std::vector<osg::Vec3f> positions;
	GeneratePosition(k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovV),
		k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovV), startRadius, numV, m_outerFovRadiusPairs, positions);

	memcpy(ptrMBPos->GetBuffer(), &positions[0], pointNum*sizeof(k3CU::Vec3f));
	m_ptrOuterSurfacePositionBuffer = k3RE::VertexBuffer::Create(k3RE::VertexBuffer::VBTS_FLOAT_3, k3RE::VertexBuffer::MANAGED,
		pointNum, ptrMBPos);
}

void CamberRadarObject::GenerateOuterFovRadiusPairs(float r)
{
	m_outerFovRadiusPairs.clear();
	m_outerFovRadiusPairs.push_back(std::pair<float,float>(0.0,r));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(10.0), r *0.9));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(20.0), r * 0.8));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(30.0), r * 0.7));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(40.0), r * 0.6));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(50.0), r * 0.5));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(60.0), r * 0.4));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(70.0), r * 0.3));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(80.0), r * 0.1));
	m_outerFovRadiusPairs.push_back(std::pair<float, float>(osg::DegreesToRadians(90.0), r * 0.01));
}

void CamberRadarObject::GenerateInnerFovRadiusPairs(float r)
{
	m_innnerFovRadiusPairs.clear();
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(0.0, r));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(10.0), r *0.9));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(20.0), r * 0.8));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(30.0), r * 0.7));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(40.0), r * 0.6));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(50.0), r * 0.5));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(60.0), r * 0.4));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(70.0), r * 0.3));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(80.0), r * 0.1));
	m_innnerFovRadiusPairs.push_back(std::pair<float, float>(k3CU::DegreesToRadians(90.0), r * 0.01));
}

int32_t CamberRadarObject::FindFovRadiusPairsIndex(float fov, const std::vector<std::pair<float, float> >& fovRadiusPairs)
{
	std::vector<std::pair<float, float> >::const_iterator itr = fovRadiusPairs.begin();
	for (; itr != fovRadiusPairs.end(); itr++)
	{
		if ((*itr).first > fov)
			return itr - fovRadiusPairs.begin();
	}

	return -1;
}

float CamberRadarObject::LerpRadius(float fov, const std::vector<std::pair<float, float> >& fovRadiusPairs)
{
	int32_t index = FindFovRadiusPairsIndex(fov, fovRadiusPairs);
	if (index > 0)
	{
		const std::pair<float, float>& preFovRadius = fovRadiusPairs[index - 1];
		const std::pair<float, float>& fovRadius = fovRadiusPairs[index];
		float factor = (fov - preFovRadius.first) / (fovRadius.first - preFovRadius.first);
		return preFovRadius.second*(1 - factor) + fovRadius.second*factor;
	}
	
	return 1.0;
}

void CamberRadarObject::GeneratePosition(float _startFovH, float _endFovH, float _startFovV, float _endFovV, int32_t startRadius,
	int32_t numV, const std::vector<std::pair<float, float> >&fovRadiusPairs, std::vector<osg::Vec3f>& positions)
{
	positions.resize((startRadius + 1) * (numV + 1));
	for (int32_t i = 0; i < (startRadius + 1); ++i)
		for (int32_t j = 0; j < (numV + 1); ++j)
		{
			float fovV = Interpolate(_startFovV, _endFovV, numV, j);
			osg::Vec3f pos = CalPos(Interpolate(_startFovH, _endFovH, startRadius, i), fovV);
			float radius = LerpRadius(fovV, fovRadiusPairs);
			positions[j * (startRadius + 1) + i] = pos *radius;
		}
}

float CamberRadarObject::Interpolate(float start, float end, float total, float val)
{
	return (start + (val / total) * (end - start));
}

k3CU::Vec3f CamberRadarObject::CalPos(float fovH, float fovV)
{
	return k3CU::Vec3f(cos(fovH) * cos(fovV), sin(fovH) * cos(fovV), sin(fovV));
}

void CamberRadarObject::GenerateInnerCurve()
{
	//surface
	m_ptrInnerSurfaceDrawable = new k3RE::PrimitiveDrawable;
	m_ptrInnerSurfaceDrawable->SetJoinInSelect(false);
	if (!m_ptrSurfaceIndex.Valid())
	{
		int32_t startRadius = m_segmentH * m_subSegmentH;
		int32_t num = m_segmentV * m_subSegmentV;
		m_ptrSurfaceIndex = GenerateSurfaceIndex(startRadius, num);
	}

	m_ptrInnerSurfaceDrawable->SetIndexBuffer(m_ptrSurfaceIndex);

	if (!m_ptrInnerSurfacePositionBuffer.Valid())
	{
		GenerateInnerSurfaceVB();
	}

	m_ptrInnerSurfaceDrawable->SetPositionBuffer(m_ptrInnerSurfacePositionBuffer);


	k3RE::DrawArraysPtr ptrSurfaceDrawArrays = new k3RE::DrawArrays(k3RS::OT_Triangle_List,
		0, m_ptrSurfaceIndex->GetIndexNumber(), 0, m_ptrInnerSurfacePositionBuffer->GetVertexNum(), true);

	m_ptrInnerSurfaceDrawable->SetDrawArrays(ptrSurfaceDrawArrays);

	//line
	m_ptrInnerLineDrawable = new k3RE::PrimitiveDrawable;
	m_ptrInnerLineDrawable->SetJoinInSelect(false);
	if (!m_ptrLineIndex.Valid())
	{
		m_ptrLineIndex = GenerateLineIndex(m_segmentH, m_segmentV, m_subSegmentH, m_subSegmentV);
	}

	m_ptrInnerLineDrawable->SetIndexBuffer(m_ptrLineIndex);
	m_ptrInnerLineDrawable->SetPositionBuffer(m_ptrInnerSurfacePositionBuffer);


	k3RE::DrawArraysPtr ptrLineDrawArrays = new k3RE::DrawArrays(k3RS::OT_Line_List,
		0, m_ptrLineIndex->GetIndexNumber(), 0, m_ptrOuterSurfacePositionBuffer->GetVertexNum(), true);

	m_ptrInnerLineDrawable->SetDrawArrays(ptrLineDrawArrays);
}

void CamberRadarObject::GenerateInnerSurfaceVB()
{
	int32_t startRadius = m_segmentH * m_subSegmentH;
	int32_t numV = m_segmentV * m_subSegmentV;

	int32_t pointNum = (startRadius + 1) * (numV + 1);

	k3CU::MemBufferPtr ptrMBPos = k3CU::MemBuffer::Create(pointNum*sizeof(k3CU::Vec3f));
	std::vector<k3CU::Vec3f> positions;
	GeneratePosition(k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovV),
		k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovV), startRadius, numV, m_innnerFovRadiusPairs, positions);

	memcpy(ptrMBPos->GetBuffer(), &positions[0], pointNum*sizeof(k3CU::Vec3f));
	m_ptrInnerSurfacePositionBuffer = k3RE::VertexBuffer::Create(k3RE::VertexBuffer::VBTS_FLOAT_3, k3RE::VertexBuffer::MANAGED,
		pointNum, ptrMBPos);
}

void CamberRadarObject::GenerateCrossSectionPosition(float _fovH, float _startFovV, float _endFovV, int32_t startRadius, int32_t numV, std::vector<k3CU::Vec3f>& positions)
{
	positions.resize((startRadius + 1) * (numV + 1));
	for (int32_t i = 0; i < (startRadius + 1); ++i)
		for (int32_t j = 0; j < (numV + 1); ++j)
		{
			float fovV = Interpolate(_startFovV, _endFovV, numV, j);
			k3CU::Vec3f pos = CalPos(_fovH, fovV);
			float radius = Interpolate(LerpRadius(fovV, m_innnerFovRadiusPairs), LerpRadius(fovV, m_outerFovRadiusPairs), startRadius, i);
			positions[j * (startRadius + 1) + i] = pos *radius;
		}
}

void CamberRadarObject::CreateLeftCrossSection()
{
	//surface
	m_ptrLeftCrossSectionSurfaceDrawable = new k3RE::PrimitiveDrawable;
	m_ptrLeftCrossSectionSurfaceDrawable->SetJoinInSelect(false);
	if (!m_ptrCrossSectionSurfaceIndex.Valid())
	{
		int32_t num = m_segmentV * m_subSegmentV;
		m_ptrCrossSectionSurfaceIndex = GenerateSurfaceIndex(10, num);
	}

	m_ptrLeftCrossSectionSurfaceDrawable->SetIndexBuffer(m_ptrCrossSectionSurfaceIndex);

	if (!m_ptrLeftCrossSectionPositionBuffer.Valid())
	{
		GenerateLeftCrossSectionSurfaceVB();
	}

	m_ptrLeftCrossSectionSurfaceDrawable->SetPositionBuffer(m_ptrLeftCrossSectionPositionBuffer);


	k3RE::DrawArraysPtr ptrSurfaceDrawArrays = new k3RE::DrawArrays(k3RS::OT_Triangle_List,
		0, m_ptrCrossSectionSurfaceIndex->GetIndexNumber(), 0, m_ptrLeftCrossSectionPositionBuffer->GetVertexNum(), true);

	m_ptrLeftCrossSectionSurfaceDrawable->SetDrawArrays(ptrSurfaceDrawArrays);

	//line
	m_ptrLeftCrossSectionLineDrawable = new k3RE::PrimitiveDrawable;
	m_ptrLeftCrossSectionLineDrawable->SetJoinInSelect(false);
	if (!m_ptrCrossSectionLineIndex.Valid())
	{
		m_ptrCrossSectionLineIndex = GenerateLineIndex(10, m_segmentV, 1, m_subSegmentV);
	}

	m_ptrLeftCrossSectionLineDrawable->SetIndexBuffer(m_ptrCrossSectionLineIndex);
	m_ptrLeftCrossSectionLineDrawable->SetPositionBuffer(m_ptrLeftCrossSectionPositionBuffer);


	k3RE::DrawArraysPtr ptrLineDrawArrays = new k3RE::DrawArrays(k3RS::OT_Line_List,
	0, m_ptrCrossSectionLineIndex->GetIndexNumber(), 0, m_ptrLeftCrossSectionPositionBuffer->GetVertexNum(), true);

	m_ptrLeftCrossSectionLineDrawable->SetDrawArrays(ptrLineDrawArrays);
}

void CamberRadarObject::GenerateLeftCrossSectionSurfaceVB()
{
	int32_t startRadius = 10;
	int32_t numV = m_segmentV * m_subSegmentV;

	int32_t pointNum = (startRadius + 1) * (numV + 1);

	k3CU::MemBufferPtr ptrMBPos = k3CU::MemBuffer::Create(pointNum*sizeof(k3CU::Vec3f));
	std::vector<k3CU::Vec3f> positions;
	GenerateCrossSectionPosition(k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovV),
		k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovV), startRadius, numV, positions);

	memcpy(ptrMBPos->GetBuffer(), &positions[0], pointNum*sizeof(k3CU::Vec3f));
	m_ptrLeftCrossSectionPositionBuffer = k3RE::VertexBuffer::Create(k3RE::VertexBuffer::VBTS_FLOAT_3, k3RE::VertexBuffer::MANAGED,
		pointNum, ptrMBPos);
}

void CamberRadarObject::GenerateRightCrossSectionSurfaceVB()
{
	int32_t startRadius = 10;
	int32_t numV = m_segmentV * m_subSegmentV;

	int32_t pointNum = (startRadius + 1) * (numV + 1);

	k3CU::MemBufferPtr ptrMBPos = k3CU::MemBuffer::Create(pointNum*sizeof(k3CU::Vec3f));
	std::vector<k3CU::Vec3f> positions;
	GenerateCrossSectionPosition(k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovH), k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_startFovV),
		k3CU::DegreesToRadians(m_ptrCamberRadarInfo->m_endFovV), startRadius, numV, positions);

	memcpy(ptrMBPos->GetBuffer(), &positions[0], pointNum*sizeof(k3CU::Vec3f));
	m_ptrRightCrossSectionPositionBuffer = k3RE::VertexBuffer::Create(k3RE::VertexBuffer::VBTS_FLOAT_3, k3RE::VertexBuffer::MANAGED,
		pointNum, ptrMBPos);
}

void CamberRadarObject::CreateRightCrossSection()
{
	//surface
	m_ptrRightCrossSectionSurfaceDrawable = new k3RE::PrimitiveDrawable;
	m_ptrRightCrossSectionSurfaceDrawable->SetJoinInSelect(false);
	if (!m_ptrCrossSectionSurfaceIndex.Valid())
	{
		int32_t num = m_segmentV * m_subSegmentV;
		m_ptrCrossSectionSurfaceIndex = GenerateSurfaceIndex(10, num);
	}

	m_ptrRightCrossSectionSurfaceDrawable->SetIndexBuffer(m_ptrCrossSectionSurfaceIndex);

	if (!m_ptrRightCrossSectionPositionBuffer.Valid())
	{
		GenerateRightCrossSectionSurfaceVB();
	}

	m_ptrRightCrossSectionSurfaceDrawable->SetPositionBuffer(m_ptrRightCrossSectionPositionBuffer);


	k3RE::DrawArraysPtr ptrSurfaceDrawArrays = new k3RE::DrawArrays(k3RS::OT_Triangle_List,
		0, m_ptrCrossSectionSurfaceIndex->GetIndexNumber(), 0, m_ptrRightCrossSectionPositionBuffer->GetVertexNum(), true);

	m_ptrRightCrossSectionSurfaceDrawable->SetDrawArrays(ptrSurfaceDrawArrays);

	//line
	m_ptrRightCrossSectionLineDrawable = new k3RE::PrimitiveDrawable;
	m_ptrRightCrossSectionLineDrawable->SetJoinInSelect(false);
	if (!m_ptrCrossSectionLineIndex.Valid())
	{
		m_ptrCrossSectionLineIndex = GenerateLineIndex(10, m_segmentV, 1, m_subSegmentV);
	}

	m_ptrRightCrossSectionLineDrawable->SetIndexBuffer(m_ptrCrossSectionLineIndex);
	m_ptrRightCrossSectionLineDrawable->SetPositionBuffer(m_ptrRightCrossSectionPositionBuffer);


	k3RE::DrawArraysPtr ptrLineDrawArrays = new k3RE::DrawArrays(k3RS::OT_Line_List,
		0, m_ptrCrossSectionLineIndex->GetIndexNumber(), 0, m_ptrRightCrossSectionPositionBuffer->GetVertexNum(), true);

	m_ptrRightCrossSectionLineDrawable->SetDrawArrays(ptrLineDrawArrays);
}


