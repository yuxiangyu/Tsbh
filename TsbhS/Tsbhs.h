#pragma once

#include <QtWidgets/QMainWindow>

#include "RtssCore.h"
#include <QtWidgets/QTreeView>
#include "zLayer.h"
#include "ui_Tsbhs.h"
//#include "TsbhPlot.h"

class Tsbhs :  public QMainWindow
{
	Q_OBJECT

public:
	Tsbhs(QWidget *parent = Q_NULLPTR);
	
	~Tsbhs();
private:
	Ui::tsbhsUI ui;
	geo3DPS* geo3dps;
	////±‡¬Î
	QTextCodec* code;
	//TsbhPlot* tsbhplot_3d;
	//TsbhPlot* tsbhplot_2d;
	int framerate;
	QTimer*                             timer_;
	//osg::ref_ptr<simVis::ViewManager>   viewMan_;
	//osg::ref_ptr<simVis::SceneManager>  sceneMan_;
	QPointer<QWindow>                   lastCreatedGlWindow_;
	
	//Õº≤„ ˜
	//QTreeView *treeView;
	QStandardItemModel* mapLayerTreeRoot;
	////Õº≤„ ˜≈‰÷√œÓ
	QStandardItem* mapLayerTreeItem;

	//QTreeView *treeView_1;
	//QTreeView *treeView_2;
	//QTreeView *treeView_3;
	//QDockWidget* m3d_view;
	//QDockWidget* m2d_view;
	//QWidget* viewWidget;
	QDockWidget* ppanel;

	//void createDockWindows();
	//≈‰÷√ Ù–‘√Ê∞Â
	void createPropertyPanel();
	void create3Dmap();
	void createContextMenu();
	
	void createBiaohuiTools();
	//≈‰÷√Õº≤„ ˜
	void configMapLayerTreeView();
	//void removeAllDock();
	//void showDock();
	QAction * currentBiaoHui = nullptr;
public slots:
	//void open3dview();
	//void open2dview();
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent* size);
	void on_action_mapconfig_triggered();
	void changeMapConfig(zMapEntity*);
	void on_addServiceLayer_action_triggered();
	void addLayer(zLayer*);
	void exit();
	void  mapLayerTreeViewClick(QModelIndex index);

	void slotCustomContextMenu(const QPoint & point);
	void on_biaohui_triggered();
	void maptreeItem_CheckChildChanged(QStandardItem*);
	void showmessage(QString message);
protected:
	virtual void paintEvent(QPaintEvent* e);
	void showMapLayerProperties(string nodename);
	double k;
signals:
	void updateLayer(zLayer*);
	void notify(QString);
	public slots:

};
