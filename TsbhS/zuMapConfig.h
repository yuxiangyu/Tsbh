#pragma once
#include <QtWidgets/qmainwindow.h>
#include "ui_mapConfig.h"
#include "zMapEntity.h"


class zuMapConfig : public QDialog
{
	Q_OBJECT
public:
	//	PromptDialog(QWidget *parent=0);
	zuMapConfig(QWidget *parent = 0);
	void setEntity(zMapEntity* entity);
	~zuMapConfig();
	zMapEntity* getEntity();
private:
	Ui::MapConfigDialog *ui;
//	geo3DPS* geo3dps;
	//µÿÕº≈‰÷√
	zMapEntity* mapentity;
	public slots:

	void changeIndex();
	void confirmMapConfig();
	void cancel();

signals:
	void mapConfiged(zMapEntity*);
};

