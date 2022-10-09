#pragma once

#include "rtsscore_global.h"
#include "gsview.h"
#include <QElapsedTimer>
#include "zLayer.h"


/************************************************************************/
/*                      geo3DPSÀà                                       */
/************************************************************************/
class RTSSCORE_EXPORT geo3DPS : public  QObject
{
	Q_OBJECT

public:
	~geo3DPS();
	//gsView* getView(bool show3d = false);
	gsView* getView();

	static geo3DPS* getInstance();
	bool addLayer(zLayer* layer);
	zLayer* getLayer(string name);
	

private:
	geo3DPS();
	static geo3DPS* _instance;
	gsView* _view;
	QList<zLayer*> layers;

public slots:
	//¸üÐÂÍ¼²ã
	void updateLayer(zLayer*);
};

