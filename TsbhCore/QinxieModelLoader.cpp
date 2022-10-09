#include "QinxieModelLoader.h"
#include "SysConst.h"
#include <osgEarth/XmlUtils>
#include <QtCore>
#include <osgEarth/ModelLayer>
#include <osgEarth/GeoTransform>
#include <osgEarth/spatialReference>
#include <osgEarth/Registry>

QinxieModelLoader::QinxieModelLoader() { group = NULL; }
QinxieModelLoader::~QinxieModelLoader() {}

void QinxieModelLoader::loadModel(std::string name)
{
	if (!group)
		return;
	this->rootFolder += "\\data\\";
	osg::ref_ptr<osgEarth::GeoTransform> xform = new osgEarth::GeoTransform();
	
	xform->setTerrain(mapNode->getTerrain());
	osgEarth::GeoPoint point(osgEarth::SpatialReference::create("wgs84") , mVecPos.x(),mVecPos.y(),mVecPos.z(), osgEarth::ALTMODE_ABSOLUTE);                 //使用绝对高，正高
	xform->setPosition(point);
			//遍历每一个文件夹
		//	QStringList fileList;
			QDir dir(QString::fromLocal8Bit(this->rootFolder.c_str()));
			dir.setFilter(QDir::Dirs);
			dir.setSorting(QDir::Name);
			QStringList list = dir.entryList();
			for (int i = 0; i < list.size(); i++)
			{
				if (list[i].indexOf(".") < 0)
				{
					QDir dir2(QString::fromLocal8Bit(this->rootFolder.c_str()) + list[i] + "/");
					dir2.setFilter(QDir::Files);
					dir2.setSorting(QDir::Name);
					QStringList list2 = dir2.entryList();
					/*for(int j=0;j<list2.size();j++)
					{ */
						 
					osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(this->rootFolder + list[i].toStdString() + "/" + list2[0].toStdString());
					if(node.valid())
					{ 
						osgEarth::Registry::shaderGenerator().run(node);
					xform->addChild(node);
					
					
						}
					/*}*/
					//break;
				}
			}
			osg::ref_ptr<osgEarth::ModelLayer> modelLayer = new osgEarth::ModelLayer();
			//ModelLayer* layer = new ModelLayer();
			modelLayer->setNode(xform);
			modelLayer->setName(name);
			group->addLayer(modelLayer);
			modelLayer->open();
			//mapNode->addChild(xform);
}

void QinxieModelLoader::ReadMetaInfoFromLocal()
{
	size_t index2 = rootFolder.rfind("\\");
	std::string strMetaInfoPath = rootFolder.substr(0, index2 == std::string::npos ? rootFolder.rfind("/") : index2);
	strMetaInfoPath += index2 == std::string::npos ? "/metadata.xml" : "\\metadata.xml";
	osg::ref_ptr<osgEarth::XmlDocument> doc = osgEarth::XmlDocument::load(strMetaInfoPath);

	if (!doc)
		return;

	std::string strSRSPos = doc->findElement("SRSOrigin")->getText();

	mStrSrs = doc->findElement("SRS")->getText();

	std::vector<std::string> vSrs=CSysConst::vStringSplit(strSRSPos,",");
	//SplitString(strSRSPos, vSrs, ",");
	str2Vec3d(vSrs, mVecPos);

}

void QinxieModelLoader::str2Vec3d(std::vector<std::string> v, osg::Vec3d &p)
{
	int pSize = v.size();
	for (int i = 0; i < pSize; i++)
	{
		p[i] = (atof(v[i].c_str()));
	}
	return;
}

void QinxieModelLoader::setLoadPosition(float lon, float lat, float alt)
{
	mVecPos.x() = lon;
	mVecPos.y() = lat;
	mVecPos.z() = alt;
}
