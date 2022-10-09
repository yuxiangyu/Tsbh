#include "RtssCore.h"
#include "MapData.h"
#include <fstream>

/************************************************************************/
/*                      geo3DPSÀà                                       */
/************************************************************************/
geo3DPS* geo3DPS::_instance = NULL;
geo3DPS::geo3DPS()
{
	_view = NULL;
}

geo3DPS::~geo3DPS()
{
	//	stepper.Stop();
	//dtCore::System::GetInstance().Stop();
	//dtQt::DeltaStepper stepper;
	osgEarth::Registry::instance()->unRefImageDataAfterApply() = false;
}

//gsView* geo3DPS::getView(bool show3d)
gsView* geo3DPS::getView()
{
	if (_view)
		return _view;
	else
	{
		//Instantiate the application and look for the config file
		//_view = new gsView(show3d);
		_view = new gsView();

		//configuring the application
		//app->Config();

		return _view;
	}
}

geo3DPS* geo3DPS::getInstance()
{
	if (!_instance)
		_instance = new geo3DPS();
	return _instance;
}

bool geo3DPS::addLayer(zLayer* layer)
{
	if (layer->name == "")
		return false;
	for (zLayer* layer1 : this->layers)
	{
		if (layer1->name == layer->name)
			return false;
	}
	this->layers.append(layer);
	return true;
}

zLayer* geo3DPS::getLayer(string name)
{
	for (zLayer* layer1 : this->layers)
	{
		if (layer1->name == name)
			return layer1;
	}
	return NULL;
}

void  geo3DPS::updateLayer(zLayer* layer)
{
	for (zLayer* layer1 : this->layers)
	{
		if (layer1->name == layer->name)
		{
			layer1 = layer;
			//¸üÐÂÍ¼²ã
			this->_view->updateLayer(layer);
		}
	}
}




