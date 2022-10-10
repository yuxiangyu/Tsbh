#include "Tsbhs.h"
#include <QtWidgets/QApplication>
#include <osgEarth/Registry>

int main(int argc, char *argv[])
{
	//osgEarth::Registry::instance()->getCapabilities();
	osg::ArgumentParser arguments(&argc, argv);
	//osgEarth::Registry::instance()->unRefImageDataAfterApply() = false;
	QApplication a(argc, argv);
	//a.setStyle(new RibbonStyle);
	//dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();
	Tsbhs w;
	w.show();
 a.exec();
 return 0;
}
