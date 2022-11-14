#pragma once
#include "rtsscore_global.h"
#include "zlayer.h"
//#include <dtCore/object.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <osg/Vec3f>


class RTSSCORE_EXPORT zeVectorLayer : public zLayer
{
public:
	zeVectorLayer();
	zeVectorLayer(std::string name);
	~zeVectorLayer(void);
	//ÑÕÉ«
	osg::Vec3f color1;
	//Í¸Ã÷¶È
	double opaticue;
	//¿í¶È
	double linewidth;
	//ÇÐÆ¬·¶Î§
	double maxrange=10e6;

	int index;
	std::string input;
	std::string imagePath;
	//Ìî³äÉ«
	osg::Vec3f color2;
	void show();
	QStringList addressname;
};

