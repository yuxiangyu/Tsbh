#include "zLayer.h"


zLayer::zLayer()
{
	showing=true;
	type = 0;
}


zLayer::~zLayer(void)
{
}

void zLayer::show()
{}

void zLayer::setShowing(bool show) {
	this->showing = show;
}

//void zLayer::setIsFirst(bool isFirst) {
//	this->isFirst = isFirst;
//}