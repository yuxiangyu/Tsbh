#include "BuildRaderCallback.h"


BuildRaderCallback::BuildRaderCallback(float fRotateSpeed, float fRotateRadius, float fRotateHeight)
{
	m_fSpeed = fRotateSpeed;
	m_fRadius = fRotateRadius;
	m_fHeight = fRotateHeight;
}

BuildRaderCallback::~BuildRaderCallback()
{

}

void BuildRaderCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{

	osg::Geode * pGeode = dynamic_cast<osg::Geode *>(node);
	osg::ref_ptr<osg::Geometry> rpGeo = dynamic_cast<osg::Geometry*> (pGeode->getDrawable(0));


	//获取顶点数组，设置连接方式
	osg::ref_ptr<osg::Vec3Array> rpVertexArray = dynamic_cast<osg::Vec3Array*>(rpGeo->getVertexArray());


	double dRotateTime = nv->getFrameStamp()->getReferenceTime(); //获取当前运行时间

	rpVertexArray->push_back(rpVertexArray->at(0));
	rpVertexArray->push_back(rpVertexArray->at(1));
	rpVertexArray->push_back(osg::Vec3(m_fRadius * cosf(dRotateTime * m_fSpeed), m_fRadius * sinf(dRotateTime * m_fSpeed), -m_fHeight));
	rpVertexArray->erase(rpVertexArray->begin());
	rpVertexArray->erase(rpVertexArray->begin());
	rpVertexArray->erase(rpVertexArray->begin());

	rpVertexArray->dirty();

	//更新轨迹
	traverse(node, nv);
}