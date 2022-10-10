#include "UShowLayerCon.h"


UShowLayerCon::UShowLayerCon(zLayer *sat,QWidget* parent, int currentview)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setStyle(QStyleFactory::create("Fusion"));
	code = QTextCodec::codecForName("gb2312");
	this->ecModel = sat;
	//this->ecSat2 = NULL;
	this->setWindowTitle(QStringLiteral("图层属性"));	
	this->currentView = currentview;
	
	init();
	connect(this, SIGNAL(updateLayer(zLayer*)), parent, SIGNAL(updateLayer(zLayer*)));
	connect(ui.table_1, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnTableItemChanged(QTableWidgetItem*)));
}



void UShowLayerCon::init()
{
	ui.table_1->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.table_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.table_1->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.table_1->horizontalHeader()->setStretchLastSection(true);
	ui.table_1->verticalHeader()->setHidden(true);
	ui.table_1->setItem(0, 0, new QTableWidgetItem(QStringLiteral("图层名称")));
	ui.table_1->setItem(0, 1, new QTableWidgetItem(QString::fromLocal8Bit(ecModel->name.c_str())));
	if (ecModel->layerType == 0)
	{
		ui.table_1->setItem(1, 0, new QTableWidgetItem(QStringLiteral("服务地址")));
		ui.table_1->setItem(1, 1, new QTableWidgetItem(QString::fromStdString(ecModel->filePath)));
		ui.table_1->setItem(2, 0, new QTableWidgetItem(QStringLiteral("服务图层名")));
		ui.table_1->setItem(2, 1, new QTableWidgetItem(QString::fromStdString(ecModel->layers)));
		//ui.table_1->item(2, 1)->setFlags(Qt::ItemIsEditable);
		connect(ui.table_1, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(rowDoubleClicked(const QModelIndex &)));
		
	}
	
	
	//ui.table_1->setItem(6,0,new QTableWidgetItem(QObject::tr("Sensor Color")));
	//ui.table_1->setItem(7,0,new QTableWidgetItem(QObject::tr("Sensor Transparency")));
	//ui.table_1->setItem(8,0,new QTableWidgetItem(QObject::tr("Deflection Angle")));
	/*ui.table_1->setItem(9, 0, new QTableWidgetItem(QObject::tr("Double Sat")));
	ui.table_1->setItem(10, 0, new QTableWidgetItem(QObject::tr("SecondSat Longitude")));
	ui.table_1->setItem(11, 0, new QTableWidgetItem(QObject::tr("Second Angle")));*/
	//box1=new QDoubleSpinBox(this);
	//
	//box1->setRange(-180, 180);
	//box1->setDecimals(4);
	//box1->setSingleStep(0.001);
	//box1->setValue(ecModel->longitude);
	//connect(box1, SIGNAL(valueChanged(double)), this, SLOT(onEditFinished()));

	//box2 = new QDoubleSpinBox(this);
	//
	//box2->setMaximum(90.000);
	//box2->setMinimum(-90.000);
	//box2->setDecimals(4);
	//box2->setSingleStep(0.001);
	//box2->setValue(ecModel->latitude);
	//connect(box2, SIGNAL(valueChanged(double)), this, SLOT(onEditFinished()));

	//box3 = new QDoubleSpinBox(this);
	//
	//box3->setMaximum(10000.0);
	//box3->setMinimum(-1000.0);
	//box3->setDecimals(2);
	//box3->setSingleStep(0.1);
	//box3->setValue(ecModel->height);
	//connect(box3, SIGNAL(valueChanged(double)), this, SLOT(onEditFinished()));

	//box4 = new QDoubleSpinBox(this);
	//
	//box4->setMaximum(1000.0);
	//box4->setMinimum(0.1);
	//box4->setDecimals(1);
	//box4->setSingleStep(0.1);
	//box4->setValue(ecModel->scale);
	//if (ecModel->type == 1)
	//	box4->setEnabled(false);
	//connect(box4, SIGNAL(valueChanged(double)), this, SLOT(onEditFinished()));

	//box5 = new QDoubleSpinBox(this);
	//
	//box5->setMaximum(360.0);
	//box5->setMinimum(0.0);
	//box5->setDecimals(1);
	//box5->setSingleStep(0.1);
	//

	//this->pSlider = new QSlider(this);
	//pSlider->setOrientation(Qt::Horizontal);
	//pSlider->setMinimum(0);
	//pSlider->setMaximum(360);
	//pSlider->setSingleStep(1);
	//
	//if (currentView == 0)
	//{ 
	//	box5->setValue(ecModel->orient);
	//	pSlider->setValue((int)ecModel->orient);
	//}
	//else
	//{ 
	//	box5->setValue(ecModel->orient2);
	//	pSlider->setValue((int)ecModel->orient2);
	//}

	//connect(box5, SIGNAL(valueChanged(double)), this, SLOT(onEditFinished()));
	////connect(box5, SIGNAL(valueChanged(int)), pSlider, SLOT(setValue(int)));
	//connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(setAngleValue(int)));
	//ui.table_1->setCellWidget(1,1,box1);
	////  模型名
	////ui.table_1->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(ecModel->modelName)));
	//lineedits = new QLineEdit(this);
	//lineedits->setText(QString::fromLocal8Bit(ecModel->modelName.c_str()));
	//ui.table_1->setCellWidget(0, 1, lineedits);
	//connect(lineedits, SIGNAL(editingFinished()), this, SLOT(onEditFinished2()));
	////  卫星姿态向量
	//ui.table_1->setCellWidget(2, 1, box2);
	////  卫星缩放大小
	//ui.table_1->setCellWidget(3, 1, box3);
	////  轨道颜色
	//ui.table_1->setCellWidget(4, 1, box4);
	//ui.table_1->setCellWidget(5, 1, box5);
	//ui.table_1->setCellWidget(6, 1, pSlider);

}

void  UShowLayerCon::rowDoubleClicked(const QModelIndex index)
{
	//QModelIndex index = ui.table_1->currentIndex();
	//if()

	if (index.isValid())

	{
		if (index.column() == 1)
		{
		
			if(index.row() == 1)
			{ 
		QString text = QInputDialog::getText(this, QStringLiteral("服务地址"),
			QStringLiteral("服务地址"), QLineEdit::Normal,
			QString::fromStdString(ecModel->filePath));
		if (!text.isEmpty())
		{ 
			ecModel->filePath = text.toStdString();
			ui.table_1->setItem(1, 1, new QTableWidgetItem(QString::fromStdString(ecModel->filePath)));

		}
			}
			else if (index.row() == 2)
			{
				QString text = QInputDialog::getText(this, QStringLiteral("服务图层"),
					QStringLiteral("服务图层"), QLineEdit::Normal,
					QString::fromStdString(ecModel->layers));
				if (!text.isEmpty())
				{
					ecModel->layers = code->fromUnicode(text);
					ui.table_1->setItem(2, 1, new QTableWidgetItem(text));

				}
			}
			
		}
	}
	
}
//
void  UShowLayerCon::OnTableItemChanged(QTableWidgetItem* item)
{
	if (!ecModel->layerType)
	{
		if (item->column() == 1)
		{
			if(!ecModel->layers.empty()&&!ecModel->filePath.empty())
			emit updateLayer(ecModel);
		}
	}
}
//
//void UShowLayerCon::setModel2(zeModel *sat2)
//{
//	this->ecModel = sat2;
//}
//
//void UShowLayerCon::onEditFinished2()
//{
//	zeModel* model2 = new zeModel();
//	model2->type = this->ecModel->type;
//	string name1 = this->ecModel->modelName;
//	model2->modelName = code->fromUnicode(this->lineedits->text());
//	model2->path = this->ecModel->path;
//	model2->longitude = box1->value();
//	model2->latitude = box2->value();
//	model2->height = box3->value();
//	model2->scale = box4->value();
//	if (currentView == 0)
//	{
//		model2->orient = box5->value();
//		model2->orient2 = this->ecModel->orient2;
//	}
//	else
//	{ 
//		model2->orient2 = box5->value();
//		model2->orient = this->ecModel->orient;
//	}
//	emit applyProperty(name1, model2);
//	this->ecModel = model2;
//}
//
//void UShowLayerCon::updateName(string name)
//{
//	this->lineedits->setText(QString::fromLocal8Bit(name.c_str()));
//}