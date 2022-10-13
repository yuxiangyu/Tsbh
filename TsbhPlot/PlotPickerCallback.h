#pragma once
#include <osgEarth/Registry>
#include <osgEarth/ObjectIndex>
#include <osgEarth/RTTPicker>
#include <QtCore/QObject>
#include "ToolProperty.h"
#include "TsbhPlot_global.h"

using namespace osgEarth;
class TsbhPlot_EXPORT PlotPickerCallback : public QObject,public osgEarth::RTTPicker::Callback
{
	Q_OBJECT
public:
	PlotPickerCallback(osg::ref_ptr<osgEarth::MapNode>mapNode);
	~PlotPickerCallback();
	void onHit(ObjectID id);
	void onMiss();
	void setPlotPickerType(int type) { m_type = type; }
	bool accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa);

protected:
	//osg::Group* root;
private:
	void installHighlighter();

	ToolProperty* m_toolpro = NULL;
signals:
	void signalCameraActor(unsigned int obejctid);

private:
	osg::Uniform*     highlightUniform;
	osg::Uniform*     TimeUniform;
	osg::ref_ptr<osgEarth::MapNode>m_mapNode;
	int m_type = 0;
};

