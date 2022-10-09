#include "zeVectorLayer.h"


zeVectorLayer::zeVectorLayer() :zLayer()
{
	this->showing=true;
	this->layerType=0;
	this->applyType=1;
}


zeVectorLayer::~zeVectorLayer(void)
{
}

zeVectorLayer::zeVectorLayer(std::string name)
{
	this->name=name;
	this->showing=true;
	this->layerType=0;
	this->applyType=1;
}
void zeVectorLayer::show()
{}