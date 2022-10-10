#pragma once


#include "ui_MainWindow.h"
#include <QtWidgets>
#include <QtCore>
#include <QApplication>
#include <qtcore/qtextcodec.h>
#include <QtGui>
#include <ActiveQt/QAxWidget>
#include <QVariant>
#include <QDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QLayout>
#include <QGridLayout>
#include "TsbhCoredoc.h"
#include <QDesktopWidget>
#include <QPropertyAnimation>

#include "nowsituation.h"
#include "MonitoringdeploymentDialog.h"
#include "KqVideosurveillanceDialog.h"
#include "TargetlabelingDialog.h"
#include "simVis/view.h"
#include "ToolHandle.h"
#include "TszsSim.h"
#include "measure.h"
#include "history.h"
#include "Monitoring.h"
#include "dateline.h"
#include "videofusion.h"
#include "../TszsData/KqDatabaseQuery.h"


//#include <StackedWidgetAnimation/StackedWidgetAnimation.h>

struct point
{
	double x;
	double y;
	double z;
};

struct Zhuji {
	int id;
	int type;
	std::vector<point> points;
	int IcoType;
};

using namespace std;
class TszsS : public QMainWindow
{
	Q_OBJECT

public:
	TszsS(QWidget *parent = 0);
	~TszsS();
	//geo3DPS* geo3dps;
	//void resizeEvent(QResizeEvent* size);
	void SetGraphicsWidget();
	//����Ƥ��
	void setHud();
	void setTopWidget();

	template<typename T>
	static void save_Datas(const T &s, std::string filename);
	template<typename T>
	static void restore_Datas(T &s, std::string filename);
	std::vector<Zhuji> getZhuJiData();
	QWidget* w;
	QWidget* plan;
	QWidget* head;

public slots:
	//void exit();
	void closeEvent(QCloseEvent *event);
	void showMonitoring();
	void showHistory();
	void showMeasure();
	void showView();
	void exitSystem();
	void setHeadWidget();
	void showTop();
	void showHead();
	void showVideoFusion();
	void showCurrentsituation();
	void showMonitoringdeployment();
	void showTargetlabeling();
	void finishSignal();

	//add by lwg 2016/6/16
	//eventType �¼�����,0���ɻ���ɡ�1���ɻ����䡢2���������䡢3����������
	//playSpeed���ű�����0.5��1��2��4��
	void slotHistorySituationShow(int eventType, float playSpeed);
private:
	Ui::MainWindow ui;
	QWidget* m2d_view;
	geo3DPS* geo3dps;
	ToolHandle* tsbhplot_3d;
	//��ʼ�ӽ�
	simVis::Viewpoint vp2;
	osgEarth::Viewpoint vp1;
	int framerate;
	QTimer* timer_;
	QTextCodec* code;
	QWidget* m3d_view;
	QPointer<QWindow> lastCreatedGlWindow_;
	int nWidth;
	int nHeight;
	QVBoxLayout* l;
	std::vector< Zhuji>  m_datas;
	

	NowSituation* nowsituation = nullptr;//��ǰ̬��
	MonitoringdeploymentDialog *m_monitoringdeployment;//��ز���
	KqVideosurveillanceDialog *m_videosurveillance;//��ز����ҷ���Ƶ���
	TargetlabelingDialog *m_targetlabeling;//Ŀ���ע
	Measure* measure = nullptr;//��������
	History* history = nullptr;//��ʷ̬��
	Monitoring* monitoring = nullptr;//���״̬
	DateLine* dateline = nullptr;//ʱ��������
	VideoFusion* videofusion = nullptr;//��Ƶ�ں�
	void connectSignals();
	void closeWidget();
	void initCamera();

	TszsSim* m_tszsSim;		//̬��չʾ����
	std::string m_currentActorId; //��ǰ̬��id
	QList<QMap<QString, QVariant>> datas;
	//init monitorCar
	void initMonitorCar();

protected:
	virtual void paintEvent(QPaintEvent* e);

signals:
	void notify(QString);
	void m_Signal();
public slots:
	void slotsCameraActor(QString str);
	void slotsDistance();
	void slotsArea();
	void slotsLinearSight();
	void slotsAzimuth();
	void slotsRadialAnalysis();
	void slotsProfileAnalysis();
};



