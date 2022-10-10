//ģ�����ԶԻ���
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
	//����
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
	//  ��ǰ��ͼ
	int  currentView;
	//  ģ����
	QLineEdit *lineedits ;
	//  ECSatellite
	zLayer *ecModel;
	//ECSatellite *ecSat2;
	//  ͸����
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
		//���ýǶ�
		//void setAngleValue(int);
		//  ����table˫��item�¼�
	//	void slot_RowDoubleClicked(const QModelIndex &index);
	//	void checkSat2(int state);
};

