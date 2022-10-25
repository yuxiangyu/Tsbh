#include "MUpdateCallback.h"



MUpdateCallback::MUpdateCallback(osg::Camera* camera, const osg::Vec3d &world)
	:m_updateCamera(camera), m_world(world)
{
	
}


MUpdateCallback::~MUpdateCallback()
{
}

void MUpdateCallback::onUpdateCamera(const osg::Camera* _camera)
{
	osg::Vec3d _eye, _center, _up;
	_camera->getViewMatrixAsLookAt(_eye, _center, _up);

	if (abs(_center.x() - m_world.x()) < 1000.0 && abs(_center.y() - m_world.y()) < 1000.0)
	{
		m_eye = _eye;
		m_center = _center;
		m_up = _up;
	}
	//qDebug() << abs(center.x() - mV3_worldP.x()) << abs(center.z() - mV3_worldP.z());

	m_updateCamera->setViewMatrixAsLookAt(m_eye, m_center, m_up);

}
