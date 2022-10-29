#include "BuildRader.h"


osg::ref_ptr<osg::Geode> CBuildRader::BuildRader(float fRadius, float fHeight)
{
	buildRaderCallback = new BuildRaderCallback(2, fRadius, fHeight);

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> rpGeom = new osg::Geometry;
	osg::ref_ptr<osg::TessellationHints> rpHints = new osg::TessellationHints;
	rpHints->setDetailRatio(0.5f);


	//��������
	osg::ref_ptr<osg::Vec3Array> rpVec3Array = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> rpVec4Array = new osg::Vec4Array;

	rpVec3Array->push_back(osg::Vec3f(0, 0, 0));
	rpVec3Array->push_back(osg::Vec3f(0, 0, -fHeight));
	rpVec3Array->push_back(osg::Vec3f(fRadius, 0, -fHeight));

	rpGeom->setVertexArray(rpVec3Array); //���ö���
	rpGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, rpVec3Array->size()));    //���ù�����ʽ  ������

	rpVec4Array->push_back(osg::Vec4f(1, 0, 0, 0.5));
	rpGeom->setColorArray(rpVec4Array);    //���ö�����ɫ
	rpGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX); //���ù�����ʽ

	rpGeom->setDataVariance(osg::Object::DYNAMIC);

	rpGeom->setUseVertexBufferObjects(true);

	rpGeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//���ɫ
	rpGeom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//͸����
	rpGeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	rpGeode->addDrawable(rpGeom.get());
	//ɨ����»ص�����
	rpGeode->addUpdateCallback(buildRaderCallback);
	return rpGeode;
}