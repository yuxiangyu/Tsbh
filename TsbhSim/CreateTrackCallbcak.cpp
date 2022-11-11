#include <osg/MatrixTransform>
#include "CreateTrackCallbcak.h"


CreateTrackCallback::CreateTrackCallback(osg::Group* root, const osgEarth::SpatialReference* srs, float lineWidth)
	:m_proot(root)
	, m_pMapSRS(srs)
	, m_fwidth(lineWidth)
	, bStart(false)
{
	//创建运动线
	m_rpLine = new AnimatedLineNode();

	m_rpLine->setColor1(osgEarth::Color::Red); // 
	m_rpLine->setColor2(osg::Vec4());   // transparent
	m_rpLine->setShiftsPerSecond(30.0);
	m_rpLine->setLineWidth(lineWidth);
	root->addChild(m_rpLine.get());
}

CreateTrackCallback::~CreateTrackCallback()
{
}

void CreateTrackCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	osg::MatrixTransform* pmtTrans = dynamic_cast<osg::MatrixTransform*> (node);
	if (pmtTrans)
	{
		osg::Matrix mtx = pmtTrans->getMatrix();
		osg::Vec3d vec3Pos = mtx.getTrans();
		// XYZ转经纬度
		osg::Vec3d vecLLH;
		m_pMapSRS->getEllipsoid()->convertXYZToLatLongHeight(vec3Pos.x(), vec3Pos.y(), vec3Pos.z(), vecLLH.y(), vecLLH.x(), vecLLH.z());
		vecLLH.x() = osg::RadiansToDegrees(vecLLH.x());
		vecLLH.y() = osg::RadiansToDegrees(vecLLH.y());
		if (!bStart)
		{
			m_startPoint = osgEarth::GeoPoint(m_pMapSRS->getGeographicSRS(), vecLLH);
			bStart = true;
		}
		else
		{
			osgEarth::GeoPoint endPoint(m_pMapSRS->getGeographicSRS(), vecLLH);
			m_rpLine->setEndPoints(m_startPoint, endPoint);
		}
		//m_proot->addChild(BuildTrack(m_Vec3LastPosition, m_Vec3CurPosition));
	}
	traverse(node, nv);
	//m_Vec3LastPosition = m_Vec3CurPosition;

}

osg::ref_ptr<osg::Geode> CreateTrackCallback::BuildTrack(osg::Vec3 m_Vec3LatPoint, osg::Vec3 m_Vec3CurPoint)
{
	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> rpGeom = new osg::Geometry;
	osg::ref_ptr<osg::TessellationHints> rpHints = new osg::TessellationHints;
	rpHints->setDetailRatio(0.5f);

	//顶点数组
	osg::ref_ptr<osg::Vec3Array> rpVec3Array = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> rpVec4Array = new osg::Vec4Array;

	rpVec3Array->push_back(m_Vec3LatPoint);
	rpVec3Array->push_back(m_Vec3CurPoint);


	rpGeom->setVertexArray(rpVec3Array); //设置顶点
	rpGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, rpVec3Array->size()));    //设置关联方式  线段

	rpVec4Array->push_back(osg::Vec4f(0, 1, 0, 1.0));
	rpGeom->setColorArray(rpVec4Array);    //设置顶点颜色
	rpGeom->setColorBinding(osg::Geometry::BIND_OVERALL); //设置关联方式

	rpGeom->setDataVariance(osg::Object::DYNAMIC);

	rpGeom->setUseVertexBufferObjects(true);

	//设置线宽
	osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(m_fwidth);
	rpGeom->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
	rpGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	rpGeom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);    //混合色
	rpGeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);    //透明度

	rpGeode->addDrawable(rpGeom.get());
	return rpGeode;
}