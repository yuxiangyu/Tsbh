//模型属性对话框
#pragma once
#include <QtWidgets>
#include "ui_modelproperty.h"
#include <QComboBox>
#include "zLayer.h"

class UShowLayerCon:public QWidget
{
	Q_OBJECT
public:
	UShowLayerCon(zLayer *sat, QWidget* parent=0,int currentview=0);
	//~UShowModelCon(void);
	//经度
	//double mm;
	//void setLon();
	void init();
	//void setModel2(zLayer *sat2);
private:
	Ui::ConProperty ui;
	QDoubleSpinBox* box1;
	QDoubleSpinBox* box2;
	QDoubleSpinBox* box3;
	QDoubleSpinBox* box4;
	QDoubleSpinBox* box5;
	QSlider *pSlider;
	//  当前视图
	int  currentView;
	//  模型名
	QLineEdit *lineedits ;
	//  ECSatellite
	zLayer *ecModel;
	//ECSatellite *ecSat2;
	//  透明度
	//QDoubleSpinBox* transparencyBox;
	//QDoubleSpinBox* transparencyBox2;

	QTextCodec* code;
signals:
	void updateLayer(zLayer *);
	//void applyProperty(string name,zeModel *);
	//void applyProperty2(zeModel *);

	public slots :
	void rowDoubleClicked(const QModelIndex index);
	void OnTableItemChanged(QTableWidgetItem*);
		//void updateName(string name);
		//设置角度
		//void setAngleValue(int);
		//  卫星table双击item事件
	//	void slot_RowDoubleClicked(const QModelIndex &index);
	//	void checkSat2(int state);
};

