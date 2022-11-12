#include "TsbhSim.h"
#include "BuildRaderCallback.h"
#include "TrailerCallback.h"
#include "CreateTrackCallbcak.h"

#include <osgDB/ReadFile>



TsbhSim::TsbhSim(osgEarth::MapNode* _mapNode)
	: m_mapNode(_mapNode)
{
}


TsbhSim::~TsbhSim()
{
}

osg::MatrixTransform* TsbhSim::LoadEntity(const std::string &strPath,
	const osg::Matrix &matrix)
{
	osg::Node* node = osgDB::readNodeFile(strPath);
	if (!node)
	{
		return nullptr;
	}
	osg::MatrixTransform* mt = new osg::MatrixTransform;
	mt->setMatrix(matrix);
	//�������ԣ����շ���
	mt->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON); 
	mt->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	mt->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	mt->getOrCreateStateSet()->setMode(GL_LIGHT1, osg::StateAttribute::ON);
	mt->addChild(node);
	return mt;
}

osg::ref_ptr<osg::Geode> TsbhSim::BuildRader(float fSpeed, float fRadius, float fHeight)
{
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
	rpGeode->addUpdateCallback(new BuildRaderCallback(fSpeed, fRadius, fHeight));
	return rpGeode;
}

osg::AnimationPath* TsbhSim::CreateAnimationPath(osg::Vec4Array* ctrl, osg::Vec3d scale)
{
	if (!m_mapNode)
	{
		return nullptr;
	}

	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	// ���ö�����ѭ��
	animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);						

	double time = 0;
	osg::Vec3d curPosition, curNextPosition;

	osg::Matrix matrix;
	osg::Quat _rotation;

	double hAngle = 0.0, vAngle = 0.0;
	for (osg::Vec4Array::iterator iterator = ctrl->begin(); iterator != ctrl->end(); ++iterator)
	{
		osg::Vec4Array::iterator iterator2 = iterator;
		++iterator2;

		// ��Ҫ�ж��ǲ����Ѿ�����
		if (iterator2 == ctrl->end()) { break; }

		// ���㵱ǰ��͵�ǰ����һ��λ��:�ɾ�γ��תΪxyz
		m_mapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ
		(
			osg::DegreesToRadians(iterator->y()),
			osg::DegreesToRadians(iterator->x()),
			iterator->z(),
			curPosition.x(), curPosition.y(), curPosition.z()
		);
		m_mapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ
		(
			osg::DegreesToRadians(iterator2->y()),
			osg::DegreesToRadians(iterator2->x()),
			iterator2->z(),
			curNextPosition.x(), curNextPosition.y(), curNextPosition.z()
		);

		// ���㴹ֱ�н�
		if (iterator->z() == iterator2->z())
		{
			vAngle = 0;
		}
		else
		{
			if (sqrt(pow(GetDis(curPosition, curNextPosition), 2)) - pow(iterator2->z() - iterator->z(), 2) == 0)
			{
				vAngle = osg::PI_2;
			}
			else
			{
				vAngle = atan((iterator2->z() - iterator->z()) / sqrt(pow(GetDis(curPosition, curNextPosition), 2))
					- pow((iterator2->z() - iterator->z()), 2));
			}

			if (vAngle >= osg::PI_2)
				vAngle = osg::PI_2;
			if (vAngle <= -osg::PI_2)
			{
				vAngle = -osg::PI_2;
			}
		}

		// ����ˮƽ�н�
		if (iterator->x() == iterator2->x())
		{
			hAngle = osg::PI_2;
		}
		else
		{
			hAngle = (iterator2->y() - iterator->y()) / (iterator2->x() - iterator->x());

			if (iterator2->x() > iterator->x())
				hAngle += osg::PI;
		}

		//��ɻ��ı任����
		m_mapNode->getMapSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(iterator->y()), osg::DegreesToRadians(iterator->x()), iterator->z(), matrix);
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), vAngle + osg::PI_2, osg::Vec3(0.0, 1.0, 0.0),
			hAngle, osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);
		animationPath->insert(time, osg::AnimationPath::ControlPoint(curPosition, matrix.getRotate(), scale));

		//����һ�����ʱ�������
		time += GetRunTime(curPosition, curNextPosition, iterator2->w());
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(curNextPosition, matrix.getRotate()));
	return animationPath.release();
}

double TsbhSim::GetRunTime(osg::Vec3d from, osg::Vec3d to, double speed)
{
	if (speed == 0)
	{
		return 1000000000;
	}
	else
	{
		return GetDis(from, to) / speed;
	}
}

double TsbhSim::GetDis(osg::Vec3d from, osg::Vec3d to)
{
	return std::sqrt((to.x() - from.x())*(to.x() - from.x()) + (to.y() - from.y())*(to.y() - from.y()) + (to.z() - from.z())*(to.z() - from.z()));
}

osg::ref_ptr<osg::Geode> TsbhSim::BuildRibbon(int size, osg::MatrixTransform* scaler, float ribbonWidth)
{
	osg::ref_ptr<osg::Geometry> rpgeom = new osg::Geometry;
	//���ö���
	osg::ref_ptr<osg::Vec3Array> rpvec3Vertex = new osg::Vec3Array(size);
	//������ɫ
	osg::ref_ptr<osg::Vec4Array> rpvec4Color = new osg::Vec4Array(size);

	for (unsigned int i = 0; i <size - 1; i += 2)
	{
		(*rpvec3Vertex)[i] = osg::Vec3(0, 0, 0);
		(*rpvec3Vertex)[i + 1] = osg::Vec3(0, 0, 0);

		float falpha = sinf(osg::PI * (float)i / (float)size);

		(*rpvec4Color)[i] = osg::Vec4(osg::Vec3f(0, 1, 0), falpha);
		(*rpvec4Color)[i + 1] = osg::Vec4(osg::Vec3f(0, 1, 0), falpha);
	}
	//�������ݶ�̬�ı�
	rpgeom->setDataVariance(osg::Object::DYNAMIC);
	//������ʾ�б���̬���²���ȫ
	rpgeom->setUseDisplayList(false);
	//ʹ��VBOģʽ
	rpgeom->setUseVertexBufferObjects(true);

	rpgeom->setVertexArray(rpvec3Vertex);

	rpgeom->setColorArray(rpvec4Color);
	rpgeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	rpgeom->addPrimitiveSet(new osg::DrawArrays(GL_QUAD_STRIP, 0, size));

	osg::ref_ptr<osg::Geode> rpgeode = new osg::Geode;
	rpgeode->addDrawable(rpgeom);
	//�ƹ⡢͸����
	rpgeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	rpgeom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	rpgeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	scaler->addUpdateCallback(new TrailerCallback(rpgeom, size, ribbonWidth));

	return rpgeode;
}

// ��������β��
osg::MatrixTransform* TsbhSim::BuildTail(osg::MatrixTransform* scaler, const osg::Vec3 &pos)
{
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(pos, 5000, 1);
	fire->setWind(osg::Vec3(1.0f, 0.0f, 0.0f));
	osg::MatrixTransform* mtfire = new osg::MatrixTransform;
	mtfire->setMatrix(osg::Matrix::rotate(osg::PI_2, osg::X_AXIS));
	mtfire->addChild(fire);
	scaler->addUpdateCallback(new TrailerCallback(fire));
	return mtfire;
}

osg::ref_ptr<osg::Group> TsbhSim::BuildHistoryRoute(osg::MatrixTransform* scaler, float lineWidth) 
{
	osg::ref_ptr<osg::Group> rpgroup = new osg::Group;

	scaler->addUpdateCallback(new CreateTrackCallback(rpgroup, m_mapNode->getMapSRS(), lineWidth));
	return rpgroup;
}
