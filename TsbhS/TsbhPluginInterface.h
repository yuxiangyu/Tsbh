#pragma once

#include <QWidget>
#include <qmainwindow.h>


class TsbhPluginInterface {
public:
	virtual QWidget *install(QMainWindow *parent) = 0;
	virtual QString getID() = 0;
	virtual QString getName() = 0;
	virtual QString getDescrition() = 0;
	public slots:
	virtual void receivemessage(QString) = 0;
};

#define QDesignerCustomWidgetInterface_iid "org.qt-project.TsbhPluginInterface"
Q_DECLARE_INTERFACE(TsbhPluginInterface, QDesignerCustomWidgetInterface_iid)
