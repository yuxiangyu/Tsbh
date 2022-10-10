#include "zuMapConfig.h"
#include "zMapEntity.h"
zuMapConfig::zuMapConfig(QWidget *parent)
	: QDialog(parent), ui(new Ui::MapConfigDialog)
{
//	geo3dps = geo3DPS::getInstance();
//	mapentity = geo3dps->getView()->getMapEntity();
	ui->setupUi(this);
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndex()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(confirmMapConfig()));
}


zuMapConfig::~zuMapConfig(void)
{
}


void zuMapConfig::changeIndex()
{
	this->mapentity->setSelected(ui->comboBox->currentIndex());
	//this->geo3dps->getView()->setSelects(this->mapentity->selected);
	if (ui->comboBox->currentIndex() != 1)
	{
		ui->lineEdit->setEnabled(false);
		ui->lineEdit_2->setEnabled(false);
		ui->lineEdit_3->setEnabled(false);
	}
	else
	{
		ui->lineEdit->setEnabled(true);
		ui->lineEdit_2->setEnabled(true);
		ui->lineEdit_3->setEnabled(true);
		
	}
	ui->lineEdit->setText(QString::fromStdString(this->mapentity->image3dURL));
	ui->lineEdit_2->setText(QString::fromStdString(this->mapentity->image2dURL));
	ui->lineEdit_3->setText(QString::fromStdString(this->mapentity->elevationURL));
}

void zuMapConfig::setEntity(zMapEntity* entity)
{
	this->mapentity = entity;
	//如果为空
	if (!this->mapentity)
		this->mapentity = new zMapEntity();
	//装载界面
	ui->comboBox->setCurrentIndex(this->mapentity->selected);
	if (this->mapentity->selected == 1)
	{
		
	
		//ui->comboBox->setCurrentIndex(2);
		ui->lineEdit->setEnabled(true);
		ui->lineEdit_2->setEnabled(true);
		ui->lineEdit_3->setEnabled(true);
		
	}
	else
	{
		ui->lineEdit->setEnabled(false);
		ui->lineEdit_2->setEnabled(false);
		ui->lineEdit_3->setEnabled(false);
	}
	ui->lineEdit->setText(QString::fromStdString(this->mapentity->image3dURL));
	ui->lineEdit_2->setText(QString::fromStdString(this->mapentity->image2dURL));
	ui->lineEdit_3->setText(QString::fromStdString(this->mapentity->elevationURL));
}

void zuMapConfig::confirmMapConfig()
{
	this->mapentity->selected = ui->comboBox->currentIndex();
	if (this->mapentity->selected == 1)
	{
		this->mapentity->image3dURL = ui->lineEdit->text().toStdString();
		this->mapentity->image2dURL = ui->lineEdit_2->text().toStdString();
		this->mapentity->elevationURL = ui->lineEdit_3->text().toStdString();
	}
	else
	{
		this->mapentity->setSelected(this->mapentity->selected);
		//this->geo3dps->getView()->setSelects(this->mapentity->selected);
	}
	this->close();
	emit mapConfiged(this->mapentity);
}
void zuMapConfig::cancel()
{
	this->close();
}

zMapEntity* zuMapConfig::getEntity()
{
	return this->mapentity;
}