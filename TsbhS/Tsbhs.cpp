#include "TSBhs.h"
#include "RtssCore.h"
#include <osgViewer/Viewer>
#include "gsView.h"
#include "zuMapConfig.h"
#include "AddLayer.h"
#include "UShowLayerCon.h"
#include "ViewWidget.h"
#include "moc_gsView.cpp"
#include "moc_RtssCore.cpp"
#include "DrawLineTool.h"
#include "PlotStyle.h"
#include "MeasureArea.h"
#include "toolhandle.h"
#include "animationLineNode.h"
#include <osgEarth/Viewpoint>
#include "shpPointModelLayer.h"
#include "shpLineModelLayer.h"

Tsbhs::Tsbhs(QWidget *parent) : QMainWindow(parent)

{
	ui.setupUi(this);
	k = 0.0;

	QIcon* ico = new QIcon("icon1.ico");
	framerate = 20;
	this->setWindowIcon(*ico);
	geo3dps = geo3DPS::getInstance();
	code = QTextCodec::codecForName("gb2312");
	setWindowState(Qt::WindowMaximized);
	setWindowTitle(QStringLiteral("XX̬�Ʊ��������ϵͳ"));
	setDockNestingEnabled(true);
	configMapLayerTreeView();

	move(QPoint(200, 200));
	QRect geom = QApplication::desktop()->availableGeometry();
	resize(2 * geom.width() / 3, 2 * geom.height() / 3);
	create3Dmap();
	createPropertyPanel();

	createBiaohuiTools();

	createContextMenu();

	//��ʱˢ��
	timer_ = new QTimer();
	// timer single shot to avoid infinite loop problems in Qt on MSVC11
	timer_->setSingleShot(true);
	connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
	if (framerate < 1)
		framerate = 1;
	timer_->start(1000 / framerate);


	connect(ui.exitApp, SIGNAL(triggered()), this, SLOT(exit()));
	connect(this, SIGNAL(updateLayer(zLayer*)), geo3dps, SLOT(updateLayer(zLayer*)));

	//loadplugin();

}

void Tsbhs::exit() {

	//this->close();
}

Tsbhs::~Tsbhs()
{
	delete timer_;
}


void Tsbhs::closeEvent(QCloseEvent *event)
{
	//disconnect(timer_, SIGNAL(timeout()), this, SLOT(update()));
	
	delete ui.dockWidget_2->widget();
	delete ui.dockWidget->widget();
	delete geo3dps;
	geo3dps = NULL;
}

void Tsbhs::createContextMenu() {

}

void Tsbhs::slotCustomContextMenu(const QPoint &point) {

	QModelIndex index = ui.treeView->currentIndex();	
	QString data = ui.treeView->model()->data(index).toString();

	if (ui.treeView->hasFocus())
	{
		QMenu *menu;
		QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";      //������ʽ��
		menu = new QMenu(this);
		menu->setStyleSheet(qss);                                         //���˵�������ʽ

		QAction *a1 = new QAction(QStringLiteral("��"));
		menu->addAction(a1);                                            //���˵������
		connect(a1, SIGNAL(triggered()), this, SLOT(openFile()));       //���Ӵ����ź���ۺ������ۺ���ʵ�ִ򿪺�Ĳ�����


		QAction *a5 = new QAction(QStringLiteral("�޸�"));
		menu->addAction(a5);

		QAction *a2 = new QAction(QStringLiteral("����"));
		menu->addAction(a2);

		QAction *a3 = new QAction(QStringLiteral("����"));
		menu->addAction(a3);
		QAction *a4 = new QAction(QStringLiteral("ɾ��"));
		menu->addAction(a4);


		menu->exec(QCursor::pos());
	}
}

//���ز��
//int Tsbhs::loadplugin()
//{
//	int count = 0;
//	QDir pluginsDir = QDir(qApp->applicationDirPath());
//
//	if (!pluginsDir.exists("plugins")) {
//		pluginsDir.mkdir("plugins");
//	}
//
//	if (!pluginsDir.cd("plugins")) return -1;
//
//
//	QStringList names;
//
//	names << "*.dll";
//
//	foreach(QString fileName, pluginsDir.entryList(names, QDir::Files))
//	{
//		QString fullpath = pluginsDir.absoluteFilePath(fileName);
//		QPluginLoader pluginLoader(fullpath);
//
//		if (!pluginLoader.load()) {
//			qDebug() << "It is not a plugin";
//			continue;
//		}
//
//		QObject *plugin = pluginLoader.instance();
//		if (plugin)
//		{
//			auto avc = qobject_cast<TsbhPluginInterface*>(plugin);
//
//			if (avc)
//			{
//				qDebug() << fileName << ":" << avc->getName() + QStringLiteral("���سɹ���");
//
//				++count;
//
//				avc->install(this);
//
//				connect(plugin, SIGNAL(sendmessage(QString)), this, SLOT(showmessage(QString)));
//				connect(this, SIGNAL(notify(QString)), plugin, SLOT(receivemessage(QString)));
//			}
//		}
//
//		//delete plugin;
//	}
//
//	//emit notify(QStringLiteral("Ⱥ������Ϣ"));
//
//
//	return count;
//}

void Tsbhs::showmessage(QString message)
{
	qDebug() << "display message:" + message;
}


void Tsbhs::on_biaohui_triggered() {

	//geo3dps->getView()->addVideo();
	PlotStyle *style = new PlotStyle();
	QAction *a5 = (QAction *)sender();

	if (currentBiaoHui) {
		if (currentBiaoHui == a5) {
			currentBiaoHui = nullptr;
			return;
		}
		currentBiaoHui->setChecked(false);
	}
	
	currentBiaoHui = a5;

	if (a5->text() == QStringLiteral("��λ����")) {
		style->type = DrawTool::DrawType::PLACE_MARK;
	}
	else if (a5->text() == QStringLiteral("����"))
	{
		style->type = DrawTool::DrawType::DRAW_LINE;
		DrawLineTool* tool = new DrawLineTool(geo3dps->getView()->get3DSceneMapNode(),geo3dps->getView()->get3dGroup());
		tsbhplot_2d->setPlot(style);
		return;
	}
	else if (a5->text() == QStringLiteral("Բ"))
	{
		style->type = DrawTool::DrawType::DRAW_CIRCLE;
	}
	else if (a5->text() == QStringLiteral("�����"))
	{
		//�����������
		//style->type = DrawTool::DrawType::DRAW_POLYGON;
		MeasureArea *aa = new MeasureArea(geo3dps->getView()->get3dGroup(), geo3dps->getView()->get3DSceneMapNode());
		//tsbhplot_3d2->setPlot(style);

		tsbhtool_3d->setTool(aa);
		return;
	}
	else if (a5->text() == QStringLiteral("����"))
	{
		style->type = DrawTool::DrawType::DRAW_RECTANGLE;
	}
	else if (a5->text() == QStringLiteral("�ۼ���"))
	{
		style->type = DrawTool::DrawType::DRAW_GATHERINGPLACE;
	}
	else if (a5->text() == QStringLiteral("����"))
	{
		style->type = DrawTool::DrawType::DRAW_GEOLUNE;
	}

	

	
}

//������Ṥ��
void Tsbhs::createBiaohuiTools() {

	QToolBar *bar = new QToolBar(this);

	bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("��λ����"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Circle.png"), QStringLiteral("Բ"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Polyline.png"), QStringLiteral("����"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/ClosePolyLine.png"), QStringLiteral("�����"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Rectangle.png"), QStringLiteral("����"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("б��ͷ"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("˫��ͷ"), this, &Tsbhs::on_biaohui_triggered);
	bar->addAction(QIcon(":/Tsbhs/Resources/ColseArc.png"), QStringLiteral("�ۼ���"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Chord.png"), QStringLiteral("����"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);

	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("������"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("����"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/AffineEdit.png"), QStringLiteral("ֱ��ͷ"), this, &Tsbhs::on_biaohui_triggered);

	bar->addSeparator();
	bar->addAction(QIcon(":/Tsbhs/Resources/AffineEdit.png"), QStringLiteral("���ѡ��"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);	

	addToolBar(bar);
}

//����3d��ͼ
void Tsbhs::create3Dmap() {


	QWidget* viewWidget = new ViewerWidget2(geo3dps->getInstance()->getView()->get3DScene());
	ui.dockWidget_2->setWidget(viewWidget);
	//lastCreatedGlWindow_ = viewWidget->windowHandle();
	//3d���
	tsbhplot_3d = new TsbhPlot(geo3dps->getInstance()->getView()->get3DScene(), geo3dps->getInstance()->getView()->get3DSceneMapNode());
	geo3dps->getView()->get3DScene()->addEventHandler(tsbhplot_3d);

	tsbhtool_3d = new ToolHandle(geo3dps->getInstance()->getView()->get3DScene(), geo3dps->getInstance()->getView()->get3DSceneMapNode());

	geo3dps->getView()->get3DScene()->addEventHandler(tsbhtool_3d);

	QWidget* viewWidget2 = new ViewerWidget2(geo3dps->getInstance()->getView()->get2DScene());
	ui.dockWidget->setWidget(viewWidget2);
	tsbhplot_2d = new TsbhPlot(geo3dps->getInstance()->getView()->get2DScene(), geo3dps->getInstance()->getView()->get2DSceneMapNode(),true);
	geo3dps->getView()->get2DScene()->addEventHandler(tsbhplot_2d);

	//2d���
	//tsbhplot_2d = new TsbhPlot(geo3dps->getView()->get2DScene());
	//geo3dps->getView()->get2DScene()->addEventHandler(tsbhplot_2d);
	//addDockWidget(Qt::RightDockWidgetArea, m2d_view);
	//tabifyDockWidget(ui.dockWidget, ui.dockWidget_2);
	

	//zLayer* layerService2 = new zLayer();
	//layerService2->filePath = "http://localhost/qgis/qgis_mapserv.fcgi.exe";
	//layerService2->layers = "world_polygon";
	//layerService2->name = "world_polygon";
	//layerService2->type = 0;  //1Ϊ��ά  0Ϊ��ά
	//layerService2->layerType = 0;
	//geo3dps->getView()->addLayer(layerService2);

	//zLayer* layerService = new zLayer();
	//layerService->filePath = "http://localhost/qgis/qgis_mapserv.fcgi.exe";
	//layerService->layers = "county_polyline";
	//layerService->name = "county_polyline";
	//layerService->type = 0;  //1Ϊ��ά  0Ϊ��ά
	//layerService->layerType = 0;
	//geo3dps->getView()->addLayer(layerService);
	//�����˶���
	osg::ref_ptr<AnimatedLineNode> line = new AnimatedLineNode();
	line->setEndPoints(GeoPoint(geo3dps->getInstance()->getView()->get3DSceneMapNode()->getMapSRS()->getGeographicSRS(),94,30,10000),
		GeoPoint(geo3dps->getInstance()->getView()->get3DSceneMapNode()->getMapSRS()->getGeographicSRS(), 124, 30, 10000));
	line->setColor1(osgEarth::Color::Red); // orange
	line->setColor2(osg::Vec4());   // transparent
	line->setShiftsPerSecond(-30.0);
	line->setLineWidth(2);
	geo3dps->getInstance()->getView()->get3dGroup()->addChild(line);

	addAirport();

	
	shpPointModelLayer* layer1 = new shpPointModelLayer("ff", "F:\\documents\\709\\Maps\\shapefile\\point\\capital_point.shp");
	layer1->imagePath = "F:\\Projects\\osgearth-osgearth-3.1\\data\\placemark32.png";
	layer1->textEncoding = "gb2312";
	layer1->addToScene(geo3dps->getInstance()->getView()->get2DSceneMapNode());


	shpPointModelLayer* layer2 = new shpPointModelLayer("ff", "F:\\documents\\709\\Maps\\shapefile\\point\\county_point.shp");
	layer2->imagePath = "F:\\Projects\\osgearth-osgearth-3.1\\data\\placemark32.png";
	layer2->textEncoding = "gb2312";
	layer2->maxrange = 10e5*3;
	layer2->addToScene(geo3dps->getInstance()->getView()->get2DSceneMapNode());
	

	/*shpLineModelLayer* layer1 = new shpLineModelLayer("province_polyline", "D:/Work/709/shuh/Maps/shapefile/polyline/province_polyline.shp");
	layer1->addToScene(geo3dps->getInstance()->getView()->get2DSceneMapNode());*/
}

//����ı��С�¼�
void Tsbhs::resizeEvent(QResizeEvent* size)
{
	QRect w2 = this->frameGeometry();
	//this->m_treeDock->setMaximumWidth(w2.width()*0.2);
	ui.dockWidget->setMinimumWidth(w2.width()*0.7);

	ui.dockWidget->setMinimumHeight(w2.height()*0.2);

	ui.dockWidget_2->setMinimumWidth(w2.width()*0.7);
	ui.dockWidget_2->setMinimumHeight(w2.height()*0.2);
	this->ppanel->setMinimumWidth(w2.width()*0.15);

}

//�����ػ�
void Tsbhs::paintEvent(QPaintEvent* e)
{
	// refresh all the views -- only repaint if the last created GL window was exposed (or got deleted).
	// This repaints on nullptr because the flag (in this app) can only be nullptr if user closed an open
	// window, and other windows that are still open are almost certainly still exposed.  We do check
	// for isExposed() on the last created window, under the presumption that once it is exposed, we
	// can safely draw on all windows.
	if (!lastCreatedGlWindow_ || lastCreatedGlWindow_->isExposed())
	{
		ViewerWidget2* viewWidget = dynamic_cast<ViewerWidget2*>(ui.dockWidget_2->widget());
		viewWidget->frame(k);
		//geo3dps->getInstance()->getView()->get3DScene();
		//qDebug() << "fffff";
		k += 0.001;
		//lastCreatedGlWindow_= m2d_view->w
	}
	timer_->start();
}

//����ͼ����
void Tsbhs::configMapLayerTreeView()
{
	QIcon iconButtton1;
	iconButtton1.addPixmap(QPixmap(QString::fromLocal8Bit(":/Tsbhs/Resources/ball")));
	QIcon iconButtton2;
	iconButtton2.addPixmap(QPixmap((QString::fromLocal8Bit(":/Tsbhs/Resources/moon"))));
	mapLayerTreeRoot = new QStandardItemModel(ui.treeView);
	mapLayerTreeRoot->setHorizontalHeaderLabels(QStringList() << QStringLiteral("ͼ������"));
	mapLayerTreeItem = new QStandardItem(iconButtton1, QStringLiteral("��ͼͼ��"));

	//��ͼ����
	mapLayerTreeRoot->appendRow(mapLayerTreeItem);
	QStandardItem* item_newModel1 = new QStandardItem(iconButtton2, QStringLiteral("��ά��ͼͼ��"));

	mapLayerTreeItem->appendRow(item_newModel1);
	QStandardItem* item_newModel2 = new QStandardItem(iconButtton2, QStringLiteral("��ά��ͼͼ��"));

	mapLayerTreeItem->appendRow(item_newModel2);
	ui.treeView->setModel(mapLayerTreeRoot);

	ui.treeView->expandAll();
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(mapLayerTreeViewClick(QModelIndex)));

	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotCustomContextMenu(const QPoint &)));
	connect(mapLayerTreeRoot, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(maptreeItem_CheckChildChanged(QStandardItem*)));

}

//�����������
void  Tsbhs::createPropertyPanel()
{
	ppanel = new QDockWidget(QStringLiteral("�������"), this);
	//	ppanel->setAllowedAreas(Qt::AllDockWidgetAreas);
	ppanel->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, ppanel);
	splitDockWidget(ui.dockWidget, ppanel, Qt::Horizontal);
	tabifyDockWidget(ui.dockWidget, ui.dockWidget_2);
	ppanel->hide();
}


//��ͼ��������
void Tsbhs::on_action_mapconfig_triggered()
{
	zuMapConfig* mapconfig = new zuMapConfig(this);
	zMapEntity* mapentity = new zMapEntity();
	mapentity->setSelected(0);
	mapconfig->setEntity(mapentity);
	mapconfig->show();
	//mapentity = mapconfig->getEntity();
	connect(mapconfig, SIGNAL(mapConfiged(zMapEntity*)), this, SLOT(changeMapConfig(zMapEntity*)));
}

//�ı��ͼ����
void Tsbhs::changeMapConfig(zMapEntity* entity)
{
	geo3dps->getView()->configMaps(entity);
}

void Tsbhs::on_addServiceLayer_action_triggered()
{
	AddLayer* addlayer = new AddLayer(this);

	addlayer->setLayerType1(0);
	addlayer->show();
}

//��Ӳ�
void Tsbhs::addLayer(zLayer* layer)
{
	if (!geo3dps->addLayer(layer))
	{
		QMessageBox::critical(this, QStringLiteral("����"), QStringLiteral("ͼ�����ظ���Ϊ��!"));
		return;
	}
	QIcon iconButtton;
	iconButtton.addPixmap(QPixmap((QString::fromLocal8Bit(":/Tsbhs/Resources/home"))));
	QStandardItem* item_newModel = new QStandardItem(iconButtton, QString::fromLocal8Bit(layer->name.c_str()));
	item_newModel->setFlags(item_newModel->flags() | Qt::ItemIsUserCheckable);
	item_newModel->setCheckState(Qt::Checked);
	if (!layer->type)
		mapLayerTreeItem->child(0)->appendRow(item_newModel);
	else
		mapLayerTreeItem->child(1)->appendRow(item_newModel);
}

void  Tsbhs::mapLayerTreeViewClick(QModelIndex index)
{
	string nodename = code->fromUnicode(index.data().toString()).toStdString();
	showMapLayerProperties(nodename);
}

void Tsbhs::showMapLayerProperties(string nodename)
{
	zLayer* layer1 = geo3dps->getLayer(nodename);
	if (layer1)
	{
		UShowLayerCon* con = new UShowLayerCon(layer1, this);
		ppanel->setWidget(con);
		ppanel->show();
	}
}

//����ͼ����������
void Tsbhs::maptreeItem_CheckChildChanged(QStandardItem* item)
{
	if (item == nullptr)
		return;
	if (item->isCheckable())
	{
		Qt::CheckState state = item->checkState();
		//int row = item->row();
		string nodename = code->fromUnicode(item->index().data().toString());
		zLayer* zlayer = geo3dps->getLayer(nodename);
		zlayer->setShowing(state);
		emit updateLayer(zlayer);
	}
}

//����άͬ��
void Tsbhs::on_action2D3Dlink_triggered()
{
	if (ui.action2D3Dlink->isChecked())
	{
		geo3dps->getInstance()->getView()->enable2D3DLinkage();
	}
	else
	{
		geo3dps->getInstance()->getView()->disable2D3DLinkage();
	}
}

// test
// ���طɻ�
void Tsbhs::addAirport()
{
	if (!tsbhsim)
	{
		tsbhsim = new TsbhSim(geo3dps->getView()->get3DSceneMapNode());
	}
	// ���طɻ�ģ��
	osg::Matrixd mtTemp = osg::Matrixd::scale(100, 100, 100)
		* osg::Matrixd::rotate(osg::PI_2, osg::Vec3(0, 0, 1));
	MatrixTransform* mtFlyself = tsbhsim->LoadEntity("D:/osg/709/shuh/Data/Model/B737.ive", mtTemp);
	if (!mtFlyself)
	{
		return;
	}
	// �����״�
	mtFlyself->addChild(tsbhsim->BuildRader(2, 500, 300).get());

	MatrixTransform* mtFly = new osg::MatrixTransform;
	mtFly->addChild(mtFlyself);
	osg::Group* group3D = geo3dps->getInstance()->getView()->get3dGroup();
	group3D->addChild(mtFly);
	//���÷ɻ�����
	/*geo3dps->getView()->get3DSceneMapNode()->getMapSRS()->getEllipsoid()
		->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(0.0), osg::DegreesToRadians(0.0), 101000, mtTemp);
	mtTemp.preMult(osg::Matrixd::scale(osg::Vec3(10, 10, 10)));
	mtFly->setMatrix(mtTemp);*/
	//���÷ɻ��ӽǸ��� �����÷���·��
	osg::ref_ptr<osg::Vec4Array> rpvaTemp = new osg::Vec4Array;
	for (size_t i = 0; i < 20; i++)
	{
		rpvaTemp->push_back(osg::Vec4(i, 0.0, 100000 + 1000 * i, 50000));
	}
	/*rpvaTemp->push_back(osg::Vec4(0.0, 0.0, 101000, 50000));
	rpvaTemp->push_back(osg::Vec4(1.0, 0.0, 102000, 50000));
	rpvaTemp->push_back(osg::Vec4(2.0, 0.0, 103000, 50000));
	rpvaTemp->push_back(osg::Vec4(3.0, 0.0, 104000, 50000));
	rpvaTemp->push_back(osg::Vec4(4.0, 0.0, 105000, 50000));
	rpvaTemp->push_back(osg::Vec4(5.0, 0.0, 106000, 50000));
	rpvaTemp->push_back(osg::Vec4(6.0, 0.0, 107000, 50000));
	rpvaTemp->push_back(osg::Vec4(7.0, 0.0, 108000, 50000));*/

	osg::AnimationPath* animationPath = tsbhsim->CreateAnimationPath(rpvaTemp, osg::Vec3d(10, 10, 10));
	mtFly->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0, 1.0));
	//�����ӽǸ���
	osgEarth::Viewpoint vp("current", 0.0, 0.0, 120000, 0, -90, 251000);
	//vp.setNode(mtFly);
	//geo3dps->getInstance()->getView()->get3dManipulator()->getViewpoint().setNode(nodeAirFly);
	//geo3dps->getInstance()->getView()->get3dManipulator()->clearViewpoint();
	geo3dps->getInstance()->getView()->get3dManipulator()->setViewpoint(vp);
	//geo3dps->getInstance()->getView()->get3dGroup()->addChild(tsbhsim->BuildRibbon(4, mtFly, 200).get());
	group3D->addChild(tsbhsim->BuildTail(mtFly, osg::Vec3(0.0, 0.0, 101000)));
	// �����ɻ���ʷ����
	group3D->addChild(tsbhsim->BuildHistoryRoute(mtFly, 1));

	
}