#include "tszss.h"
#include "simQt/ViewWidget.h"
#include "simUtil/StatusText.h"
#include "simCore/String/TextReplacer.h"
#include "simCore/String/Utils.h"
#include "simUtil/Replaceables.h"
#include "simVis/Compass.h"
#include "simUtil/MapScale.h"
#include "transcoder.h"
#include "DrawGeometry.h"
#include "CameraActor.h"
#include "loadingwidget.h"
#include "CameraDevice.h"
#include "plotpickercallback.h"
#include <QThread>
#include <QDebug>
#include "kqwaitdlg.h"
#include "MeasureLine.h"
#include "MeasureArea.h"
#include "MeasureAzimuth.h"
#include "MeasureRadialAnalysis.h"
#include "MeasureProfileAnalysis.h"
#include "MeasureLinearSight.h"


TszsS::TszsS(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	tsbhplot_3d = nullptr;
	framerate = 30;
	geo3dps = geo3DPS::getInstance();
	tsbhplot_3d = new ToolHandle(geo3dps->getView()->get3DScene());
	
	geo3dps = geo3DPS::getInstance();
	code = QTextCodec::codecForName("gb2312");

	//	this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏 
	//setWindowState(Qt::WindowMaximized);
	setWindowState(Qt::WindowNoState);

	//this->setWindowState(Qt::WindowMaximized); //界面最大化 

	SetGraphicsWidget();
	//定时刷新
	timer_ = new QTimer();
	// timer single shot to avoid infinite loop problems in Qt on MSVC11;
	connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
	timer_->setSingleShot(true);
	if (framerate < 1)
		framerate = 1;
	timer_->start(1000 / framerate);
	//setWindowState(Qt::WindowFullScreen);
	QTimer::singleShot(0, this, SLOT(showFullScreen()));
	setTopWidget();
	setHud();
	setHeadWidget();
	head->hide();
	string sdir = Transcoder::qstr2str(qApp->applicationDirPath());
	zLayer* layer = new zLayer();
	layer->name = "dem";
	layer->layerType = 2;
	layer->filePath = sdir + "/testregion/xj/l13/xj.tif";
	geo3dps->getView()->addLayer(layer);
	connectSignals();

	m_monitoringdeployment = new MonitoringdeploymentDialog();
	m_videosurveillance = new KqVideosurveillanceDialog();
	m_targetlabeling = new TargetlabelingDialog();

	m_tszsSim = new TszsSim(geo3dps->getView()->get3DScene());
	m_currentActorId = "";
	vp2=simVis::Viewpoint("current2", 84.9, 39.63, 1000, 0.0, -30.0, 100000);
	vp1 = simVis::Viewpoint("current2", 84.9, 39.63, 1000, 0.0, -90.0, 100000);
	//viewer3d->setViewpoint(vp);
	/*m_datas = getZhuJiData();*/

	
}

TszsS::~TszsS()
{
	delete timer_;
	delete m_monitoringdeployment;
	delete m_videosurveillance;
	delete m_targetlabeling;
	delete m_tszsSim;
	delete KqDatabaseQuery::getInstacne();
}
void TszsS::SetGraphicsWidget()
{
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	w = centralWidget();
	w->setStyleSheet("QWidget{background:black;}");
	m3d_view = new simQt::ViewWidget(geo3dps->getView()->get3DScene());
	m2d_view = new simQt::ViewWidget(geo3dps->getView()->get2DScene());

	QHBoxLayout* l = new QHBoxLayout(w);
	l->addWidget(m2d_view);
	l->addWidget(m3d_view);
	m2d_view->hide();
	
}

void TszsS::setHud()
{
	simVis::View* superHudView = new simVis::View();
	superHudView->setUpViewAsHUD(geo3dps->getView()->get3DScene());
	geo3dps->getView()->get3DScene()->getViewManager()->addView(superHudView);
	superHudView->getOrCreateHUD()->addCullCallback(new osgEarth::InstallCameraUniform());
	simCore::TextReplacerPtr textReplacer(new simCore::TextReplacer());
	//textReplacer->addReplaceable(new simUtil::AzimuthVariable(mainView));
	textReplacer->addReplaceable(new simUtil::AltitudeVariable(geo3dps->getView()->get3DScene()));
	textReplacer->addReplaceable(new simUtil::LatitudeVariable(geo3dps->getView()->get3DScene()));
	textReplacer->addReplaceable(new simUtil::LongitudeVariable(geo3dps->getView()->get3DScene()));
	osg::ref_ptr<simUtil::StatusTextNode> statusText = new simUtil::StatusTextNode(textReplacer);
	statusText->setStatusSpec("Lat:\t%LAT%\tAlt:\t%ALT%\tLon:\t%LON%\t \t \n");
	//superHudView->getOrCreateHUD()->addChild(statusText.get());
	osg::MatrixTransform* xform1 = new osg::MatrixTransform();
	xform1->addChild(statusText.get());
	xform1->setMatrix(osg::Matrix::translate(osg::Vec3f(5, 15, 0.f)));
	superHudView->getOrCreateHUD()->addChild(xform1);

	osg::ref_ptr<simVis::CompassNode> compass = new simVis::CompassNode("picture/compass1.png");
	compass->setActiveView(geo3dps->getView()->get3DScene());

	compass->setMatrix(osg::Matrix::translate(osg::Vec3f(nWidth - 100, nHeight * 15 / 16 - 100, 0.f)));

	// Wrap it in a transform to allow it to tie into the unified HUD easily
	osg::MatrixTransform* xform = new osg::MatrixTransform();
	xform->addChild(compass.get());
	superHudView->getOrCreateHUD()->addChild(xform);

	osg::ref_ptr<simUtil::MapScale> mapScale = new simUtil::MapScale;
	mapScale->setView(geo3dps->getView()->get3DScene());

	osg::MatrixTransform* xform2 = new osg::MatrixTransform;
	xform2->setMatrix(osg::Matrix::translate(osg::Vec3f(nWidth / 2, 0, 0.f)));
	xform2->addChild(mapScale.get());
	superHudView->getOrCreateHUD()->addChild(xform2);
	//osg::ref_ptr<simVis::ViewManagerLogDbAdapter> logDb_ = new simVis::ViewManagerLogDbAdapter;
	//logDb_->install(geo3dps->getView()->get3DScene()->getViewManager());
}

void TszsS::closeEvent(QCloseEvent *event)
{
	delete geo3dps;
	geo3dps = NULL;
}

//窗体重绘
void TszsS::paintEvent(QPaintEvent* e)
{
	// refresh all the views -- only repaint if the last created GL window was exposed (or got deleted).
	// This repaints on nullptr because the flag (in this app) can only be nullptr if user closed an open
	// window, and other windows that are still open are almost certainly still exposed.  We do check
	// for isExposed() on the last created window, under the presumption that once it is exposed, we
	// can safely draw on all windows.
	//if (!lastCreatedGlWindow_ || lastCreatedGlWindow_->isExposed())
	//{
	//geo3dps->getView()->get2DScene()->getViewManager()->frame();
	geo3dps->getView()->get3DScene()->getViewManager()->frame();
	//lastCreatedGlWindow_= m2d_view->w
	//}
	timer_->start();
}

//系统头部标题栏
void TszsS::setTopWidget()
{
	plan = new QWidget(w);
	plan->setObjectName("plan");
	plan->showFullScreen();

	nowsituation = new NowSituation(plan);
	dateline = new DateLine(plan);
	history = new History(plan);
	monitoring = new Monitoring(plan);
	measure = new Measure(plan);
	connect(measure, SIGNAL(emit_distanceSignal()), this, SLOT(slotsDistance()));
	connect(measure, SIGNAL(emit_areaSignal()), this, SLOT(slotsArea()));
	connect(measure, SIGNAL(emit_LinearSightSignal()), this, SLOT(slotsLinearSight()));
	connect(measure, SIGNAL(emit_azimuthSignal()), this, SLOT(slotsAzimuth()));
	connect(measure,SIGNAL(emit_profileAnalysis()),this,SLOT(slotsProfileAnalysis()));
	videofusion = new VideoFusion(plan);

	dateline->datetimeWidget->show();
	plan->setGeometry(0, 0, nWidth, nHeight / 16);
	::SetWindowLong((HWND)plan->winId(), GWL_EXSTYLE, GetWindowLong((HWND)plan->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes((HWND)plan->winId(), 255, 190, LWA_ALPHA | LWA_COLORKEY);

	QPushButton* bu1 = new QPushButton(plan);
	bu1->setGeometry(150, 20, 140, 44);
	bu1->setText(QStringLiteral("监控状态"));
	

	QPushButton* bu2 = new QPushButton(plan);
	bu2->setGeometry(380, 22, 140, 44);
	bu2->setText(QStringLiteral("当前态势"));

	QPushButton* bu3 = new QPushButton(plan);
	bu3->setGeometry(1420, 22, 140, 44);
	bu3->setText(QStringLiteral("历史态势"));

	QPushButton* cutButton = new QPushButton(plan);
	cutButton->setGeometry(1630, 22, 140, 44);
	cutButton->setText(QStringLiteral("视图切换"));
	cutButton->setObjectName("cutButton");

	QPushButton* exitButton = new QPushButton(plan);
	exitButton->setGeometry(1860, 22, 44, 44);
	exitButton->setObjectName("exitButton");

	QPushButton* setButton = new QPushButton(plan);
	setButton->setGeometry(1800, 22, 44, 44);
	setButton->setObjectName("setButton");
	//标题
	QLabel* plantitle = new QLabel(QString::fromLocal8Bit("XX态势展示系统"), plan);
	plantitle->setGeometry(631, 1, 664, 71);
	plantitle->setAlignment(Qt::AlignHCenter);
	plantitle->setObjectName("plantitle");

	//连接槽，转入connectSignals()中
	connect(bu1, SIGNAL(clicked()), this, SLOT(showMonitoring()));
	connect(bu2, SIGNAL(clicked()), this, SLOT(showCurrentsituation()));
	connect(bu3, SIGNAL(clicked()), this, SLOT(showHistory()));
	connect(cutButton, SIGNAL(clicked()), this, SLOT(showView()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(exitSystem()));
	connect(setButton, SIGNAL(clicked()), this, SLOT(showHead()));

	plan->setFocusPolicy(Qt::ClickFocus);
	plan->setStyleSheet("QWidget#plan {border-image:url(:/tszss/picture/header.png);}\
						 QPushButton{border-image: url(:/tszss/picture/box-header.png);background:transparent;font-family: \"Microsoft YaHei\";color:white;font-size:26px;font-weight:bold}\
                         QPushButton:hover {border-image: url(:/tszss/picture/box-header.png);color:rgb(0, 161, 248);}\
		                 QPushButton:focus {border-image: url(:/tszss/picture/boxpressed.png);color:white;}\
						 QPushButton#exitButton{border-image:url(:/tszss/picture/navigation/exit.png);background:transparent;}\
						 QPushButton:hover#exitButton{border-image:url(:/tszss/picture/navigation/exithover.png);}\
						 QPushButton:pressed#exitButton{border-image:url(:/tszss/picture/navigation/exitpressed.png);}\
						 QPushButton#setButton{border-image:url(:/tszss/picture/navigation/set.png);background:transparent;}\
						 QPushButton:hover#setButton{border-image:url(:/tszss/picture/navigation/sethover.png);}\
						 QPushButton:pressed#setButton{border-image:url(:/tszss/picture/navigation/setpressed.png)}\
						 QLabel{background:transparent;color:white;font-family: \"Microsoft YaHei\";font-size:40px;font-weight:bold}");
	
}

std::vector<Zhuji>  TszsS::getZhuJiData()
{
	std::vector<Zhuji> Zhujis;
	Zhuji data;
	QList<QMap<QString, QVariant>> datas = KqDatabaseQuery::getInstacne()->getTableData("ATT_Zhuji");
	for (int i = 0; i < datas.size(); i++)
	{
		data.id = datas.at(i)["OID"].toInt();
		data.type = datas.at(i)["TYPE"].toInt();
		QString param = datas.at(i)["PARAM"].toString();
		QStringList Points = param.split(";");
		std::vector<point> points;
		for (int j = 1; j < Points.size();j++)
		{
			QStringList pointslist = Points.at(j).split(",");
			point pointdata;
			for (int z = 0; z < pointslist.size(); z++)
			{
				if (z == 0)
				{
					pointdata.x = pointslist.at(z).toDouble();
				}
				if (z == 1)
				{
					pointdata.y = pointslist.at(z).toDouble();
				}
				if (z == 2)
				{
					pointdata.z = pointslist.at(z).toDouble();
				}
			}
			points.push_back(pointdata);
		}
		data.points = points;
		data.IcoType = datas.at(i)["ICOTYPE"].toInt();
		Zhujis.push_back(data);
	}
	return Zhujis;
}

void TszsS::setHeadWidget()
{
	head = new QWidget(w);
	head->setObjectName("head");
	head->showFullScreen();

	head->setGeometry(0, 0, nWidth, nHeight / 16);
	::SetWindowLong((HWND)head->winId(), GWL_EXSTYLE, GetWindowLong((HWND)head->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes((HWND)head->winId(), 255, 190, LWA_ALPHA | LWA_COLORKEY);

	QPushButton* btn1 = new QPushButton(head);
	btn1->setGeometry(150, 22, 140, 44);
	btn1->setText(QStringLiteral("目标标注"));

	QPushButton* btn2 = new QPushButton(head);
	btn2->setGeometry(380, 22, 140, 44);
	btn2->setText(QStringLiteral("监控部署"));

	QPushButton* btn3 = new QPushButton(head);
	btn3->setGeometry(1420, 22, 140, 44);
	btn3->setText(QStringLiteral("测量分析"));

	QPushButton* btn4 = new QPushButton(head);
	btn4->setGeometry(1650, 22, 140, 44);
	btn4->setText(QStringLiteral("视频融合"));

	QPushButton* returnButton = new QPushButton(head);
	returnButton->setGeometry(1830, 22, 44, 44);
	returnButton->setObjectName("returnButton");

	//标题
	QLabel* headtitle = new QLabel(QString::fromLocal8Bit("XX态势展示系统"), head);
	headtitle->setGeometry(631, 1, 664, 71);
	headtitle->setAlignment(Qt::AlignHCenter);
	headtitle->setObjectName("headtitle");

	connect(returnButton, SIGNAL(clicked()), this, SLOT(showTop()));
	connect(btn4, SIGNAL(clicked()), this, SLOT(showVideoFusion()));
	connect(btn3, SIGNAL(clicked()), this, SLOT(showMeasure()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(showMonitoringdeployment()));
	connect(btn1, SIGNAL(clicked()), this, SLOT(showTargetlabeling()));

	head->setFocusPolicy(Qt::ClickFocus);
	head->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	head->setStyleSheet("QWidget#head {border-image:url(:/tszss/picture/header.png);}\
						QPushButton{border-image: url(:/tszss/picture/box-header.png);background:transparent;color:white;font-family: \"Microsoft YaHei\";font-size:25px;font-weight:bold}\
                        QPushButton:hover {border-image: url(:/tszss/picture/boxhover.png);color:rgb(0, 161, 248);}\
		                QPushButton:focus {border-image: url(:/tszss/picture/boxpressed.png);color:white;}\
						QPushButton#returnButton{border-image:url(:/tszss/picture/navigation/return.png); }\
						QPushButton:hover#returnButton{border-image:url(:/tszss/picture/navigation/returnhover.png);}\
						QPushButton:pressed#returnButton{border-image:url(:/tszss/picture/navigation/returnpressed.png);}\
						QLabel{background:transparent;color:white;font-family: \"Microsoft YaHei\";font-size:40px;font-weight:bold}");
}


void TszsS::connectSignals()
{
	bool isConnect = false;
	isConnect = connect(history, SIGNAL(sigPlaySituation(int, float)), this, SLOT(slotHistorySituationShow(int, float)));
}

void TszsS::closeWidget()
{
	monitoring->monitoringWidget->close();
	monitoring->video->videoWidget->close();
	monitoring->video->widget1->close();
	monitoring->video->widget2->close();
	monitoring->video->widget3->close();
	monitoring->video->widget4->close();
	nowsituation->nowSituationWidget->close();
	nowsituation->situationvideo->situationvideoWidget->close();
	history->historyWidget->close();
	history->player->playWidget->close();
	history->dateWidget->close();
	history->situationvideo->situationvideoWidget->close();
	m_monitoringdeployment->close();
	m_videosurveillance->close();
	m_targetlabeling->close();
	measure->mWidget->close();
	videofusion->videofusionWidget->close();
	videofusion->setvideofusion->setVFTabWidget->close();
}

void TszsS::initCamera()
{  
	datas = KqDatabaseQuery::getInstacne()->getTableData("ATT_CameraLocation");
	CameraActor * camera = new CameraActor(geo3dps->getView()->get3DScene(), geo3dps->getView()->getLod(), tsbhplot_3d);
	DrawGeometry *ptrCircle = new DrawGeometry(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	connect(camera, SIGNAL(signalCameraActor(QString)), this, SLOT(slotsCameraActor(QString)));
	camera->setCirclePtr(ptrCircle);
	camera->setCameraState(datas);
	camera->initCameraState();
	m_datas = getZhuJiData();
	int polygonIndex = 0;
	for (vector<Zhuji>::const_iterator iter = m_datas.begin(); iter != m_datas.end(); iter++)
	{
		//ptrCircle->createPolygon();
		for (vector<point>::const_iterator it = iter->points.begin(); it != iter->points.end(); it++)
		{
			ptrCircle->addPolygonPoint(it->x, it->y, it->z, polygonIndex);
		}
		ptrCircle->drawPolygon(polygonIndex);
		polygonIndex++;
	}
}

template<typename T>
void  TszsS::save_Datas(const T &s, std::string filename)
{
	// make an archive
	std::ofstream ofs(filename);
	if (ofs.good())
	{
		{
			boost::archive::xml_oarchive oa(ofs);
			oa << BOOST_SERIALIZATION_NVP(s);
		}
		ofs.close();
	}
}

template<typename T>
void  TszsS::restore_Datas(T &s, std::string filename)
{
	// open the archive
	std::ifstream ifs(filename, ios::in);
	if (ifs.good())
	{
		{//添加这个括号,控制boost::archive::xml_iarchive的域 保证先执行它的析构函数
			boost::archive::xml_iarchive ia(ifs);
			try
			{
				ia >> BOOST_SERIALIZATION_NVP(s);
			}
			catch (boost::archive::archive_exception const& e)
			{
				ifs.close();
				return;
			}
		}
		//只有boost::archive::xml_iarchive的析构正常调用,才执行文件流关闭操作
		ifs.close();
	}
}

//系统转换
void TszsS::showTop()
{
	head->hide();
	plan->show();
	closeWidget();
}

void TszsS::showHead()
{
	head->show();
	plan->hide();

	if (m2d_view->isHidden())
	{
	}
	else
	{
		m2d_view->hide();
	}
	closeWidget();
}

//视图切换
void TszsS::showView()
{
	if (m2d_view->isHidden())
	{
		m2d_view->show();

		//geo3dps->getView()->get2dManipulator()->setViewpoint(vp1, 3);
	}
	else
	{
		m2d_view->hide();
	}
	closeWidget();
}

//退出系统
void TszsS::exitSystem()
{
	this->close();
	closeWidget();
	dateline->datetimeWidget->close();
}

//监控状态
void TszsS::showMonitoring()
{
	geo3dps->getView()->get3DScene()->setViewpoint(vp2, 3);
	closeWidget();
	m2d_view->hide();

	if (datas.isEmpty())
	{

		KQWaitDlg wait(QString::fromLocal8Bit(""));
		initCamera();
		wait.WaitFor([&]()
		{
			KQProgressDisplayControl displayControl(&wait);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			for (int i = 0; i<10; ++i)
			{
				QThread::sleep(1);
				displayControl.setStep((i + 1) * 10);
				displayControl.setStepMsg(QString::fromLocal8Bit("当前进度%1/100").arg((i + 1) * 10));
			}
		}, KQWaitDlg::RingsProgress);

	}
	monitoring->monitoringWidget->show();
	QPropertyAnimation* animation = new QPropertyAnimation(monitoring->monitoringWidget, "geometry");
	animation->setDuration(300);
	animation->setStartValue(QRect(-508, 296, 350, 550));
	animation->setEndValue(QRect(24, 296, 350, 550));
	animation->start();
	monitoring->video->videoWidget->show();
	QPropertyAnimation* animation1 = new QPropertyAnimation(monitoring->video->videoWidget, "geometry");
	animation1->setDuration(300);
	animation1->setStartValue(QRect(1920, 296, 484, 368));
	animation1->setEndValue(QRect(1412, 296, 484, 368));
	animation1->start();
}

//当前态势
void TszsS::showCurrentsituation()
{	
	geo3dps->getView()->get3DScene()->setViewpoint(vp2, 3);
	closeWidget();
	m2d_view->hide();

	nowsituation->nowSituationWidget->show();
	QPropertyAnimation* animation = new QPropertyAnimation(nowsituation->nowSituationWidget, "geometry");
	animation->setDuration(300);
	animation->setStartValue(QRect(-508, 296, 350, 550));
	animation->setEndValue(QRect(24, 296, 350, 550));
	animation->start();

	nowsituation->situationvideo->situationvideoWidget->show();
	QPropertyAnimation* animation2 = new QPropertyAnimation(nowsituation->situationvideo->situationvideoWidget, "geometry");
	animation2->setDuration(300);
	animation2->setStartValue(QRect(1920, 296, 312, 496));
	animation2->setEndValue(QRect(1576, 296, 312, 496));
	animation2->start();

	//initMonitorCar();

	//MonitorCar monitor;
	//monitor.id = "500000";
	//monitor.lat = 39.30;
	//monitor.lon = 85.14;
	//monitor.alt = 1000;
	//monitor.m_azimuth = 45;
	//monitor.m_elevation = 60;
	//monitor.m_cameraRange = 3000;
	//monitor.m_cameraAngle = 40;
	//
	////监测车运动
	//std::vector<TrackPoint> trackPoints;
	//trackPoints.push_back(TrackPoint(39.50, 85.25, 1000, 2 * 60));
	//trackPoints.push_back(TrackPoint(39.70, 85.37, 1000, 2 * 60));

	//m_tszsSim->addMonitorCarAnimation(&monitor, trackPoints);
	//m_tszsSim->tetherCamera(monitor.id, monitor.lat - 45, monitor.lon - 45, monitor.alt + 2000.0);


	Airship airship;
	airship.scale = 2;
	airship.id = "600000";
	airship.lat = 39.30;
	airship.lon = 85.14;
	airship.alt = 5000;
	airship.m_azimuth = 45;
	airship.m_elevation = -90;
	airship.m_range  = 5000;
	airship.m_horizontalwidth = 40;
	airship.m_verticalwidth = 40;

	//监测车运动
	std::vector<TrackPoint> trackPoints;
	trackPoints.push_back(TrackPoint(39.50, 85.25, 1000, 2 * 60));
	trackPoints.push_back(TrackPoint(39.70, 85.37, 1000, 2 * 60));

	m_tszsSim->addAirshipAnimation(&airship, trackPoints);
	m_tszsSim->tetherCamera(airship.id, airship.lat - 45, airship.lon - 45, airship.alt + 2000.0);

	/*CPulseRadar pulseRadar;
	pulseRadar.id = "radar0002";
	pulseRadar.lon = 85.40;
	pulseRadar.lat = 40.16;
	pulseRadar.alt = 1000;
	pulseRadar.m_range = 6000;
	pulseRadar.m_azimuth = 70;
	pulseRadar.m_elevation = 60;
	pulseRadar.m_horizontalwidth = 45;
	pulseRadar.m_verticalwidth = 45;
	m_tszsSim->addPulseRadarActorToPlatform(&pulseRadar);
	m_tszsSim->tetherCamera(pulseRadar.id, pulseRadar.lat - 45, pulseRadar.lon - 45, pulseRadar.alt + 2000.0);*/

}

//历史态势
void TszsS::showHistory()
{
	geo3dps->getView()->get3DScene()->setViewpoint(vp2, 3);
	history->setGeo3dps(geo3dps);
	closeWidget();
	m2d_view->hide();

	history->historyWidget->show();
	QPropertyAnimation* animation = new QPropertyAnimation(history->historyWidget, "geometry");
	animation->setDuration(300);
	animation->setStartValue(QRect(-508, 296, 350, 550));
	animation->setEndValue(QRect(24, 296, 350, 550));
	animation->start();

	history->player->playWidget->show();
	QPropertyAnimation* animation1 = new QPropertyAnimation(history->player->playWidget, "geometry");
	animation1->setDuration(300);
	animation1->setStartValue(QRect(24, -164, 350, 90));
	animation1->setEndValue(QRect(24, 205, 350, 90));
	animation1->start();

	history->situationvideo->situationvideoWidget->show();
	QPropertyAnimation* animation2 = new QPropertyAnimation(history->situationvideo->situationvideoWidget, "geometry");
	animation2->setDuration(300);
	animation2->setStartValue(QRect(1920, 296, 312, 496));
	animation2->setEndValue(QRect(1576, 296, 312, 496));
	animation2->start();
}

//监控部署
void TszsS::showMonitoringdeployment()
{
	if (m_monitoringdeployment->isVisible())
	{
	}
	else 
	{
		closeWidget();
		//监控部署
		m_monitoringdeployment->setWindowOpacity(0.7);//透明度0.7
		m_monitoringdeployment->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Tool | Qt::FramelessWindowHint);//界面在顶端，无边框
		m_monitoringdeployment->show();
		QPropertyAnimation *animation = new QPropertyAnimation(m_monitoringdeployment, "geometry");
		animation->setDuration(300);//设置持续时间
		animation->setStartValue(QRect(-400, 200, 327, 422));//(距离左边，距离上面，，)
		animation->setEndValue(QRect(10, 200, 327, 422));
		animation->start();

		//视频监控
		m_videosurveillance->setWindowOpacity(0.7);//透明度0.7
		m_videosurveillance->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Tool | Qt::FramelessWindowHint);//界面在顶端，无边框
		m_videosurveillance->show();
		QPropertyAnimation *animation1 = new QPropertyAnimation(m_videosurveillance, "geometry");
		animation1->setDuration(300);//设置持续时间
		animation1->setStartValue(QRect(2300, 200, 380, 471));//(距离左边，距离上面，，)
		animation1->setEndValue(QRect(1500, 200, 380, 471));
		animation1->start();
	}
}

//目标标注
void TszsS::showTargetlabeling()
{
	//DrawCircle* circle = new DrawCircle(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	//tsbhplot_3d->setTool(circle);

	if (m_targetlabeling->isVisible())
	{
	}
	
	else
	{
		closeWidget();

		//目标标注
		m_targetlabeling->setWindowOpacity(0.7);//透明度0.7
		m_targetlabeling->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Tool | Qt::FramelessWindowHint);//界面在顶端，无边框
		m_targetlabeling->show();
		QPropertyAnimation *animation = new QPropertyAnimation(m_targetlabeling, "geometry");
		animation->setDuration(300);//设置持续时间
		animation->setStartValue(QRect(-600, 200, 356, 525));//(距离左边，距离上面，，)
		animation->setEndValue(QRect(10, 200, 356, 525));
		animation->start();
	}	
}

//测量分析
void TszsS::showMeasure()
{
	closeWidget();

	measure->mWidget->show();

	QPropertyAnimation* animation = new QPropertyAnimation(measure->mWidget, "geometry");
	animation->setDuration(300);
	animation->setStartValue(QRect(-508, 136, 350, 594));
	animation->setEndValue(QRect(24, 136, 350, 594));
	animation->start();
}

void TszsS::showVideoFusion()
{
	closeWidget();

	videofusion->videofusionWidget->show();
	QPropertyAnimation* animation = new QPropertyAnimation(videofusion->videofusionWidget, "geometry");
	animation->setDuration(300);
	animation->setStartValue(QRect(-508, 296, 350, 550));
	animation->setEndValue(QRect(24, 296, 350, 550));
	animation->start();

	videofusion->setvideofusion->setVFTabWidget->show();
	QPropertyAnimation* animation1 = new QPropertyAnimation(videofusion->setvideofusion->setVFTabWidget, "geometry");
	animation1->setDuration(300);
	animation1->setStartValue(QRect(1920, 296, 296, 403));
	animation1->setEndValue(QRect(1548, 296, 296, 403));
	connect(animation, SIGNAL(finished()), this, SLOT(on_closeWidget()));
	animation->start();
	animation1->start();
}
void TszsS::finishSignal()
{
	emit m_Signal();
}

void TszsS::slotHistorySituationShow(int eventType, float playSpeed)
{
	if (m_currentActorId != "")
	{
		m_tszsSim->removeActorById(m_currentActorId);
	}
	
	//特效配置文件路径
	string confighPath = QCoreApplication::applicationDirPath().toStdString() + "/config";
	switch (eventType)
	{
	case 0:
	case 1:
	{
		if (eventType == 0)
		{
			confighPath = confighPath + "/takeOff.xml";
		}
		else
		{
			confighPath = confighPath + "/aircraftLand.xml";
		}
		//读取xml配置信息 
		FlightActor flightActor;
		restore_Datas(flightActor, confighPath);
		flightActor.setType2(flightActor.type2);
		flightActor.scale = 5;
		m_tszsSim->addFlightActorToPlatform(&flightActor, playSpeed);
		m_tszsSim->tetherCamera(flightActor.id, flightActor.lat - 45, flightActor.lon - 45, flightActor.alt + 2000.0);

		m_currentActorId = flightActor.id;

		break;
	}
	case 2:
	{
		//导弹发射
		confighPath = confighPath + "/missileLaunch.xml";
		//读取xml配置信息 
		Missilelaunch missileLaunch;
		restore_Datas(missileLaunch, confighPath);
		missileLaunch.scale = 3;
		m_tszsSim->addMissilelaunchActorToPlatform(&missileLaunch, playSpeed);
		m_tszsSim->tetherCamera(missileLaunch.id, missileLaunch.lat - 45, missileLaunch.lon - 45, missileLaunch.alt + 2000.0);

		m_currentActorId = missileLaunch.id;

		break;
	}
	case 3:
	{
		//导弹遇靶
		confighPath = confighPath + "/missile.xml";
		//读取xml配置信息 
		MissileActor missileactor;
		restore_Datas(missileactor, confighPath);
		missileactor.scale = 3;
		m_tszsSim->addMisslieActorToPlatform(&missileactor, playSpeed);
		m_tszsSim->tetherCamera(missileactor.id, missileactor.lat - 45, missileactor.lon - 45, missileactor.alt + 2000.0);

		m_currentActorId = missileactor.id;

		break;
	}
	case 4:
	{
		// 导弹脱靶

		confighPath = confighPath + "/missileloss.xml";
		//读取xml配置信息 
		MissileActor missileactor;
		restore_Datas(missileactor, confighPath);
		missileactor.scale = 3;
		m_tszsSim->addLossMisslieActorToPlatform(&missileactor, playSpeed);
		m_tszsSim->tetherCamera(missileactor.id, missileactor.lat - 45, missileactor.lon - 45, missileactor.alt + 2000.0);

		m_currentActorId = missileactor.id;

		break;
	}
	default:
	{
		m_currentActorId = "";
		break;
	}
	}
}
void TszsS::slotsCameraActor(QString str)
{
	QString str1 = str;
	monitoring->video->widget1->setID(str1);
	monitoring->video->widget2->setID(str1);
	monitoring->video->widget3->setID(str1);
	monitoring->video->widget4->setID(str1);

}

void TszsS::initMonitorCar()
{
	//读取监控表数据
	QList<QMap<QString, QVariant> > listmap = KqDatabaseQuery::getInstacne()->getTableData("ATT_MonitoringVehicle");
	for (auto rowmap : listmap)
	{
		//数据解析
		MonitorCar monitorInfo;
		monitorInfo.id = rowmap.find("ID").value().toString().toLocal8Bit();
		monitorInfo.name = rowmap.find("NAME").value().toString().toLocal8Bit();
		monitorInfo.lat = rowmap.find("LATITUDE").value().toDouble();
		monitorInfo.lon = rowmap.find("LONGITUDE").value().toDouble();
		monitorInfo.alt = rowmap.find("HEIGHT").value().toDouble();
		monitorInfo.yaw = rowmap.find("ORIENTATION").value().toDouble();
		monitorInfo.pitch = rowmap.find("ATTITUDEPITCH").value().toDouble();
		monitorInfo.roll = rowmap.find("ATTITUDEROLL").value().toDouble();
		monitorInfo.m_cameraRange = rowmap.find("RANGE").value().toDouble() * 1000;
		double startAngle = rowmap.find("STARTINGANGEL").value().toDouble();
		double endAngle = rowmap.find("ENDANGEL").value().toDouble();
		monitorInfo.m_azimuth = startAngle + (endAngle - startAngle) / 2;
		monitorInfo.m_elevation = rowmap.find("POINTPITCH").value().toDouble();
		monitorInfo.m_cameraAngle = endAngle - startAngle;

		//添加监控车到地图
		m_tszsSim->addMonitorCarActorToPlatform(&monitorInfo);
	}
}
void TszsS::slotsDistance()
{
	MeasureLine* line = new MeasureLine(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(line);
}
void TszsS::slotsArea()
{
	MeasureArea* area = new MeasureArea(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(area);
}
void TszsS::slotsAzimuth()
{
	MeasureAzimuth* azimuth = new MeasureAzimuth(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(azimuth);
}
void TszsS::slotsLinearSight()
{
	MeasureLinearSight* linear = new MeasureLinearSight(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(linear);
}


void TszsS::slotsRadialAnalysis()
{
	MeasureRadialAnalysis* radial = new MeasureRadialAnalysis(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(radial);
}

void TszsS::slotsProfileAnalysis()
{
	MeasureProfileAnalysis* profile = new MeasureProfileAnalysis(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DScene()->getSceneManager()->getMapNode());
	tsbhplot_3d->setTool(profile);
}
