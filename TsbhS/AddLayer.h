#pragma once

#include <QtWidgets/qmainwindow.h>
#include "ui_AddLayer.h"
#include "zLayer.h"

class AddLayer : public QDialog
{
	Q_OBJECT

public:
	AddLayer(QWidget *parent = Q_NULLPTR);
	~AddLayer();
	void setLayerType1(int type1);

private:
	Ui::AddLayerDialog* ui;
	int type1;

public slots:

	void cancel();
	void addlayer();
signals:
	void addLayer(zLayer*);
	
};
