#pragma once
#include <QTimer>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/MultiTouchTrackballManipulator>

#include <osgDB/ReadFile>

#include <osgQt/GraphicsWindowQt>

#include <iostream>
#include <QLayout>

class ViewerWidget2 : public QWidget, public osgViewer::CompositeViewer
{
	Q_OBJECT
public:
	ViewerWidget2(osgViewer::View* view,QWidget* parent = 0, Qt::WindowFlags f = 0, osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::CompositeViewer::SingleThreaded) : QWidget(parent, f)
	{
		setThreadingModel(threadingModel);

		// disable the default setting of viewer.done() by pressing Escape.
		setKeyEventSetsDone(0);
		setQuitEventSetsDone(false);
		QWidget* widget1 = addViewWidget(createGraphicsWindow(0, 0, 100, 100), view);
		

		this->layout()->addWidget(widget1);

		connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
		_timer.start(10);
	}

	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw, osgViewer::View* view)
	{
		
		addView(view);

		osg::Camera* camera = view->getCamera();
		camera->setGraphicsContext(gw);

		const osg::GraphicsContext::Traits* traits = gw->getTraits();

		camera->setViewport(new osg::Viewport(0, 0, gw->getTraits()->width, gw->getTraits()->height));
		if (gw->getTraits()->height != 0)
			camera->setProjectionMatrixAsPerspective(30.0f, gw->getTraits()->width / gw->getTraits()->height, 1.0f, 10000.0f);
		camera->setDrawBuffer(gw->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT);
		camera->setReadBuffer(gw->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT);

		gw->setTouchEventsEnabled(true);
		return gw->getGLWidget();
	}

	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name = "", bool windowDecoration = false)
	{
		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->windowName = name;
		traits->windowDecoration = windowDecoration;
		traits->x = x;
		traits->y = y;
		traits->width = w;
		traits->height = h;
		traits->doubleBuffer = true;
		traits->alpha = ds->getMinimumNumAlphaBits();
		traits->stencil = ds->getMinimumNumStencilBits();
		traits->sampleBuffers = ds->getMultiSamples();
		traits->samples = ds->getNumMultiSamples();

		return new osgQt::GraphicsWindowQt(traits.get());
	}

	virtual void paintEvent(QPaintEvent* /*event*/)
	{
		frame();
	}

protected:

	QTimer _timer;
};