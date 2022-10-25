#pragma once
#include <osgEarth/EarthManipulator>

class MUpdateCallback : public osgEarth::EarthManipulator::UpdateCameraCallback
{
public:
	MUpdateCallback(osg::Camera* camera, const osg::Vec3d &world);

	~MUpdateCallback();

	void onUpdateCamera(const osg::Camera* _camera);

private:
	osg::Vec3d m_eye;
	osg::Vec3d m_center;
	osg::Vec3d m_up;
	osg::Vec3d m_world;
	osg::Camera* m_updateCamera;

};

