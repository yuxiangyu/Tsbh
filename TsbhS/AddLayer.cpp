#include "AddLayer.h"
#include "zLayer.h"

AddLayer::AddLayer(QWidget *parent)
	: QDialog(parent), ui(new Ui::AddLayerDialog)
{
	ui->setupUi(this);

	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addlayer()));
	connect(this, SIGNAL(addLayer(zLayer*)), parent, SLOT(addLayer(zLayer*)));
}

AddLayer::~AddLayer()
{
}

void AddLayer::cancel()
{
	this->close();
}

void AddLayer::addlayer()
{
	if (!ui->lineEdit->text().isEmpty())
	{
		zLayer* layer = new zLayer();
		layer->name = ui->lineEdit->text().toLocal8Bit();
		layer->type = ui->comboBox->currentIndex();
		layer->layerType = type1;
		emit addLayer(layer);
	}
	this->close();
}

void AddLayer::setLayerType1(int type1)
{
	this->type1 = type1;
}