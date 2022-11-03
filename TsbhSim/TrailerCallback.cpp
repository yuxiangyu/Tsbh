#include <osg/MatrixTransform>
#include "TrailerCallback.h"


TrailerCallback::TrailerCallback(osg::Geometry* ribbon, int size, float ribbonWidth)
{
	m_opGeometryRibbon = ribbon;
	m_nsize = size;
	m_fwidth = ribbonWidth;
}

void TrailerCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	osg::MatrixTransform* pmtTrans = dynamic_cast<osg::MatrixTransform*> (node);
	if (pmtTrans && m_opGeometryRibbon.valid())
	{
		osg::Matrix mtx = pmtTrans->getMatrix();
		osg::Vec3Array* pvec3Vertex = dynamic_cast<osg::Vec3Array*>(m_opGeometryRibbon->getVertexArray());

		for (unsigned int i = 0; i<m_nsize - 3; i += 2)
		{
			(*pvec3Vertex)[i] = (*pvec3Vertex)[i + 2];
			(*pvec3Vertex)[i + 1] = (*pvec3Vertex)[i + 3];
		}

		(*pvec3Vertex)[m_nsize - 2] = osg::Vec3(0.0f, -m_fwidth, 0.0f)* mtx;
		(*pvec3Vertex)[m_nsize - 1] = osg::Vec3(0.0f, m_fwidth, 0.0f)* mtx;

		pvec3Vertex->dirty();
		m_opGeometryRibbon->dirtyBound();
	}
	traverse(node, nv);
}

TrailerCallback::~TrailerCallback()
{

}