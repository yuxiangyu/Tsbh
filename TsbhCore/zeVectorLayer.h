#pragma once
#include "rtsscore_global.h"
#include "zlayer.h"
//#include <dtCore/object.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>


class RTSSCORE_EXPORT zeVectorLayer : public zLayer
{
public:
	zeVectorLayer();
	zeVectorLayer(std::string name);
	~zeVectorLayer(void);
	//��ɫ
	osg::Vec3f color1;
	//͸����
	double opaticue;
	//����
	double linewidth;
	//��Ƭ��Χ
	double maxrange;

	int index;
	std::string input;
	std::string imagePath;
	//���ɫ
	osg::Vec3f color2;
	void show();
	QStringList addressname;
};

