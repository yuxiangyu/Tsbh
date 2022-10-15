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
//#include "DrawTool.h"


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
	setWindowTitle(QStringLiteral("XX态势标绘与推演系统"));
	setDockNestingEnabled(true);
	configMapLayerTreeView();

	move(QPoint(200, 200));
	QRect geom = QApplication::desktop()->availableGeometry();
	resize(2 * geom.width() / 3, 2 * geom.height() / 3);
	create3Dmap();
	createPropertyPanel();

	createBiaohuiTools();

	createContextMenu();

	//定时刷新
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
                QMenu::separator{height:1px;background:#757575;}";      //设置样式表
		menu = new QMenu(this);
		menu->setStyleSheet(qss);                                         //给菜单设置样式

		QAction *a1 = new QAction(QStringLiteral("打开"));
		menu->addAction(a1);                                            //给菜单添加项
		connect(a1, SIGNAL(triggered()), this, SLOT(openFile()));       //连接触发信号与槽函数（槽函数实现打开后的操作）


		QAction *a5 = new QAction(QStringLiteral("修改"));
		menu->addAction(a5);

		QAction *a2 = new QAction(QStringLiteral("上移"));
		menu->addAction(a2);

		QAction *a3 = new QAction(QStringLiteral("下移"));
		menu->addAction(a3);
		QAction *a4 = new QAction(QStringLiteral("删除"));
		menu->addAction(a4);


		menu->exec(QCursor::pos());
	}
}

//加载插件
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
//				qDebug() << fileName << ":" << avc->getName() + QStringLiteral("加载成功！");
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
//	//emit notify(QStringLiteral("群发的消息"));
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

	if (a5->text() == QStringLiteral("点位文字")) {
		style->type = DrawTool::DrawType::PLACE_MARK;
	}
	else if (a5->text() == QStringLiteral("折线"))
	{
		style->type = DrawTool::DrawType::DRAW_LINE;
		//DrawLineTool* tool = new DrawLineTool(geo3dps->getView()->get3DSceneMapNode(),geo3dps->getView()->get3dGroup());
	}
	else if (a5->text() == QStringLiteral("圆"))
	{
		style->type = DrawTool::DrawType::DRAW_CIRCLE;
	}
	else if (a5->text() == QStringLiteral("多边形"))
	{
		style->type = DrawTool::DrawType::DRAW_POLYGON;
	}
	else if (a5->text() == QStringLiteral("矩形"))
	{
		style->type = DrawTool::DrawType::DRAW_RECTANGLE;
	}
	else if (a5->text() == QStringLiteral("聚集区"))
	{
		style->type = DrawTool::DrawType::DRAW_GATHERINGPLACE;
	}
	else if (a5->text() == QStringLiteral("弓形"))
	{
		style->type = DrawTool::DrawType::DRAW_GEOLUNE;
	}

	tsbhplot_3d->setPlot(style);

	
}

//创建标会工具
void Tsbhs::createBiaohuiTools() {

	QToolBar *bar = new QToolBar(this);

	bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("点位文字"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Circle.png"), QStringLiteral("圆"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Polyline.png"), QStringLiteral("折线"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/ClosePolyLine.png"), QStringLiteral("多边形"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Rectangle.png"), QStringLiteral("矩形"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("斜箭头"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("双剪头"), this, &Tsbhs::on_biaohui_triggered);
	bar->addAction(QIcon(":/Tsbhs/Resources/ColseArc.png"), QStringLiteral("聚集区"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->addAction(QIcon(":/Tsbhs/Resources/Chord.png"), QStringLiteral("弓形"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);

	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("搜索区"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/EPointMeasure.png"), QStringLiteral("扇形"), this, &Tsbhs::on_biaohui_triggered);
	//bar->addAction(QIcon(":/Tsbhs/Resources/AffineEdit.png"), QStringLiteral("直箭头"), this, &Tsbhs::on_biaohui_triggered);

	bar->addSeparator();
	bar->addAction(QIcon(":/Tsbhs/Resources/AffineEdit.png"), QStringLiteral("标绘选择"), this, &Tsbhs::on_biaohui_triggered)->setCheckable(true);
	bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);	

	addToolBar(bar);
}

//创建3d地图
void Tsbhs::create3Dmap() {


	QWidget* viewWidget = new ViewerWidget2(geo3dps->getInstance()->getView()->get3DScene());
	ui.dockWidget_2->setWidget(viewWidget);
	//lastCreatedGlWindow_ = viewWidget->windowHandle();
	//3d标绘
	tsbhplot_3d = new TsbhPlot(geo3dps->getInstance()->getView()->get3DScene(), geo3dps->getInstance()->getView()->get3DSceneMapNode());
	geo3dps->getView()->get3DScene()->addEventHandler(tsbhplot_3d);
//	geo3dps->getView()->get3DScene()->addEventHandler(tsbhplot_3d);
	//geo3dps->getView()->get3DScene()->inser
	QWidget* viewWidget2 = new ViewerWidget2(geo3dps->getInstance()->getView()->get2DScene());
	ui.dockWidget->setWidget(viewWidget2);

	//2d标绘
	//tsbhplot_2d = new TsbhPlot(geo3dps->getView()->get2DScene());
	//geo3dps->getView()->get2DScene()->addEventHandler(tsbhplot_2d);
	//addDockWidget(Qt::RightDockWidgetArea, m2d_view);
	//tabifyDockWidget(ui.dockWidget, ui.dockWidget_2);

}

//窗体改变大小事件
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

//窗体重绘
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

//配置图层树
void Tsbhs::configMapLayerTreeView()
{
	QIcon iconButtton1;
	iconButtton1.addPixmap(QPixmap(QString::fromLocal8Bit(":/Tsbhs/Resources/ball")));
	QIcon iconButtton2;
	iconButtton2.addPixmap(QPixmap((QString::fromLocal8Bit(":/Tsbhs/Resources/moon"))));
	mapLayerTreeRoot = new QStandardItemModel(ui.treeView);
	mapLayerTreeRoot->setHorizontalHeaderLabels(QStringList() << QStringLiteral("图层配置"));
	mapLayerTreeItem = new QStandardItem(iconButtton1, QStringLiteral("地图图层"));

	//地图服务
	mapLayerTreeRoot->appendRow(mapLayerTreeItem);
	QStandardItem* item_newModel1 = new QStandardItem(iconButtton2, QStringLiteral("三维地图图层"));

	mapLayerTreeItem->appendRow(item_newModel1);
	QStandardItem* item_newModel2 = new QStandardItem(iconButtton2, QStringLiteral("二维地图图层"));

	mapLayerTreeItem->appendRow(item_newModel2);
	ui.treeView->setModel(mapLayerTreeRoot);

	ui.treeView->expandAll();
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(mapLayerTreeViewClick(QModelIndex)));

	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotCustomContextMenu(const QPoint &)));
	connect(mapLayerTreeRoot, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(maptreeItem_CheckChildChanged(QStandardItem*)));

}

//创建属性面板
void  Tsbhs::createPropertyPanel()
{
	ppanel = new QDockWidget(QStringLiteral("属性面板"), this);
	//	ppanel->setAllowedAreas(Qt::AllDockWidgetAreas);
	ppanel->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, ppanel);
	splitDockWidget(ui.dockWidget, ppanel, Qt::Horizontal);
	tabifyDockWidget(ui.dockWidget, ui.dockWidget_2);
	ppanel->hide();
}


//底图服务配置
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

//改变地图配置
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

//添加层
void Tsbhs::addLayer(zLayer* layer)
{
	if (!geo3dps->addLayer(layer))
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层名重复或为空!"));
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

//控制图层树的显隐
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

//二三维同步
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