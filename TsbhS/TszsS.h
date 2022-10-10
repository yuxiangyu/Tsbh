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
	//设置皮肤
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
	//eventType 事件类型,0：飞机起飞、1：飞机降落、2：导弹发射、3：导弹遇靶
	//playSpeed播放倍数，0.5、1、2、4；
	void slotHistorySituationShow(int eventType, float playSpeed);
private:
	Ui::MainWindow ui;
	QWidget* m2d_view;
	geo3DPS* geo3dps;
	ToolHandle* tsbhplot_3d;
	//初始视角
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
	

	NowSituation* nowsituation = nullptr;//当前态势
	MonitoringdeploymentDialog *m_monitoringdeployment;//监控部署
	KqVideosurveillanceDialog *m_videosurveillance;//监控部署右方视频监控
	TargetlabelingDialog *m_targetlabeling;//目标标注
	Measure* measure = nullptr;//测量分析
	History* history = nullptr;//历史态势
	Monitoring* monitoring = nullptr;//监控状态
	DateLine* dateline = nullptr;//时间日期栏
	VideoFusion* videofusion = nullptr;//视频融合
	void connectSignals();
	void closeWidget();
	void initCamera();

	TszsSim* m_tszsSim;		//态势展示仿真
	std::string m_currentActorId; //当前态势id
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



