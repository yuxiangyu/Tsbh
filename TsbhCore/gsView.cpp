#include "gsview.h"
#include "astro.h"
#include <QDebug>
#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <assert.h>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarth/AutoClipPlaneHandler>
#include <osgEarthDrivers/sky_simple/SimpleSkyOptions>
#include <osgEarth/Map>
#include <osgEarth/XYZ>
#include <osgEarth/TMS>
#include <osgEarth/ImageLayer>
#include <osgEarth/WMS>
#include <osgEarth/GDAL>
#include <osgEarth/CullingUtils>
#include <osgEarth/GLUtils>
#include <osgEarth/ThreeDTilesLayer>
#include "qinxiemodelloader.h"
#include <osgEarth/ModelLayer>
#include "zLayer.h"
#include <osgEarth/MBTiles>
#include <osgEarth/Composite>
#include "SysConst.h"
#include <stdlib.h>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/PlaceNode>
#include <osgEarth/optional>
//#include <osgEarth/Distance>
//shp加载测试
#include <osgEarth/OGRFeatureSource>
#include <osgEarth/FeatureImageLayer>

//wmts
#include <osgEarth/ImageLayer>

//#include "MyGISServer.h"


using namespace osgEarth;
using namespace osgEarth::Util;

/*************************************
* Brief     : 用于二三维仿真场景视角同步
*************************************/
Darg2Dto3DHandle::Darg2Dto3DHandle()
{
}

Darg2Dto3DHandle::~Darg2Dto3DHandle()
{
	//qDebug() << "~Darg2Dto3DHandle()";
}

Darg2Dto3DHandle::Darg2Dto3DHandle(osgEarth::EarthManipulator * view)
{
	this->view = view;
	active = false;
}

bool Darg2Dto3DHandle::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	//if (!active)
	//	return false;
	osgViewer::View *pViewer = dynamic_cast<osgViewer::View*>(&aa);
	if (pViewer == NULL)
	{
		return false;
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		osgEarth::EarthManipulator* mi = dynamic_cast<osgEarth::EarthManipulator*>(pViewer->getCameraManipulator());
		this->view->setViewpoint(mi->getViewpoint());
		return true;
	}
	return false;
}

/*************************************
* Brief     : 仿真场景视图
*************************************/
//gsView::gsView(bool show3D)
gsView::gsView()
{
	initEarth();
	make2dMap();

	//if (!show3D) {
	//	make2dMap();
	//}
	//config3DMap();
}

gsView::~gsView()
{
}

void gsView::initEarth()
{
	//simCore::checkVersionThrow();

	/// set up the registry so the SDK can find platform models
	//simExamples::configureSearchPaths();
	osg::ref_ptr<Map> map = new Map();

	osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	cacheOptions.rootPath() = "cache";
	//map->removeLayer(map->getLayerAt(0));
	map->setCache(osgEarth::CacheFactory::create(cacheOptions));
	//map->setCachePolicy(osgEarth::CachePolicy::CACHE_ONLY);
	
	///*ArcGISOptions imagery;
	// imagery.url() = "http://s*/ervices.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/";
	/*ArcGISServerImageLayer* layer = new ArcGISServerImageLayer();
	layer->setName("simdis.imagery");
	layer->setURL("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/");
	layer->options().cachePolicy() = CachePolicy::NO_CACHE;
	map->addLayer(layer);*/
	
	/*WMSImageLayer* wms = new WMSImageLayer();
	wms->setURL("http://172.16.3.16:8699/KQGis/rest/services/huangpi_tif4326/MapServer/WMS");
	wms->setFormat("png");
	wms->setLayers("nexrad-n0r");
	wms->setSRS("EPSG:4326");
	wms->setTransparent(true);
	wms->options().cachePolicy() = CachePolicy::NO_CACHE;
	map->addLayer(wms);*/

	MBTilesImageLayer* baseLayer = new MBTilesImageLayer();
	baseLayer->setName("Whole Earth");
	baseLayer->setURL("resources/lowResEarth.mbtiles");
	map->addLayer(baseLayer);
	
	viewer3d = new osgViewer::View();
	//viewMan_3d = new simVis::ViewManager();
	viewer3dNode = new MapNode(map);
	// Note that the logarithmic depth buffer is not installed
	
	// disable the default ESC-to-quit event:
	//viewer3d->setKeyEventSetsDone(0);
	//viewer3d->setQuitEventSetsDone(false);
	viewer3d->setUpViewInWindow(100, 100, 1600, 900, 0u);
	// we need a map.
	//osg::ref_ptr<osgEarth::Map> map = simExamples::createDefaultExampleMap();

	// A scene manager that all our views will share.
	//sceneMan_3d = new simVis::SceneManager();
	//sceneMan_3d->setMap(map.get());
	//viewer3d->setNavigationMode(simVis::NAVMODE_GLOBESPIN);
	//viewer3d->setNavigationMode(simVis::NAVMODE_GIS);

	mainip3d = new EarthManipulator;
	mainip3d->getSettings()->setThrowingEnabled(true);
	mainip3d->getSettings()->setMinMaxPitch(-90, -10);
    mainip3d->getSettings()->setMinMaxDistance(30.0, DBL_MAX);
	mainip3d->getSettings()->bindMouse(EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
	mainip3d->getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP);
	mainip3d->getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_DOWN);
	
	viewer3d->setCameraManipulator(mainip3d);
	viewer3d->setSceneData(viewer3dNode);
	//viewer3d->installDebugHandlers();
	//viewer3d->getSceneManager()->getScenario()->setLODScaleFactor
	viewer3d->addEventHandler(new osgViewer::LODScaleHandler);
	//dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer3d->getCameraManipulator())
	//	->getSettings()->setTerrainAvoidanceEnabled(false);
	viewer3d->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
	osgEarth::Drivers::SimpleSky::SimpleSkyOptions skyOptions;
	skyOptions.atmosphericLighting() = false;
	skyOptions.ambient() = 0.5f;
	skyOptions.exposure() = 2.0f;
	//sceneMan_3d->setSkyNode(osgEarth::SkyNode::create(skyOptions));
	osgEarth::insertGroup(osgEarth::SkyNode::create(skyOptions), viewer3dNode->getParent(0));
	viewer3d->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(viewer3dNode));
	osgEarth::Viewpoint vp("current", 116.4, 39.5, 0.0, 0.0, -90.0, 10000000);
	mainip3d->setViewpoint(vp);
	//viewer3d->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
	// Whenever the focus manager gets a focus event, change the scale to point to it
	//mapScale->bindToFocusManager(viewer3d->getFocusManager());
	//targetGroup_3D2=sceneMan_3d->getOrCreateAttachPoint("group3d");

	targetGroup_3D = new osg::Group();
	
	viewer3dNode->addChild(targetGroup_3D);
	lod = new osg::LOD();
	targetGroup_3D->addChild(lod);
	GLUtils::setGlobalDefaults(targetGroup_3D->getOrCreateStateSet());
}

/**
 * Brief     : 创建第二个三维球场景，主要用于小场景的显示
 * Returns   : void
 * Author    : GKK 2022/09/22
 */
void gsView::initEarth2()
{
	/// set up the registry so the SDK can find platform models
	//simExamples::configureSearchPaths();
	//osg::ref_ptr<Map> map = new Map();

	//osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	//cacheOptions.rootPath() = "cache";
	////map->removeLayer(map->getLayerAt(0));
	//map->setCache(osgEarth::CacheFactory::create(cacheOptions));
	////map->setCachePolicy(osgEarth::CachePolicy::CACHE_ONLY);

	//MBTilesImageLayer* baseLayer = new MBTilesImageLayer();
	//baseLayer->setName("Whole Earth");
	//baseLayer->setURL("resources/textures/lowResEarth.mbtiles");
	//map->addLayer(baseLayer);

	//viewer3d_2 = new simVis::View();
	//viewMan_3d_2 = new simVis::ViewManager();
	//// Note that the logarithmic depth buffer is not installed

	//// disable the default ESC-to-quit event:
	//viewMan_3d_2->getViewer()->setKeyEventSetsDone(0);
	//viewMan_3d_2->getViewer()->setQuitEventSetsDone(false);
	//viewer3d_2->setUpViewInWindow(100, 100, 1600, 900, 0u);
	//// we need a map.
	////osg::ref_ptr<osgEarth::Map> map = simExamples::createDefaultExampleMap();

	//// A scene manager that all our views will share.
	//sceneMan_3d_2 = new simVis::SceneManager();
	//sceneMan_3d_2->setMap(map.get());
	////viewer3d->setNavigationMode(simVis::NAVMODE_GLOBESPIN);
	//viewer3d_2->setNavigationMode(simVis::NAVMODE_GIS);

	//mainip3d_2 = viewer3d_2->getEarthManipulator();
	//mainip3d_2->getSettings()->setThrowingEnabled(true);
	//mainip3d_2->getSettings()->setMinMaxPitch(-90, -10);
	//mainip3d_2->getSettings()->setMinMaxDistance(30.0, DBL_MAX);
	//mainip3d_2->getSettings()->bindMouse(EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
	//viewer3d_2->setSceneManager(sceneMan_3d_2.get());

	//viewMan_3d_2->addView(viewer3d_2);

	////viewer3d->installDebugHandlers();
	////viewer3d->getSceneManager()->getScenario()->setLODScaleFactor
	//viewer3d_2->addEventHandler(new osgViewer::LODScaleHandler);
	////dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer3d->getCameraManipulator())
	////	->getSettings()->setTerrainAvoidanceEnabled(false);
	//viewer3d_2->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
	//osgEarth::Drivers::SimpleSky::SimpleSkyOptions skyOptions;
	//skyOptions.atmosphericLighting() = false;
	//skyOptions.ambient() = 0.5f;
	//skyOptions.exposure() = 2.0f;
	//sceneMan_3d_2->setSkyNode(osgEarth::SkyNode::create(skyOptions));

	//viewer3d_2->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(sceneMan_3d_2->getMapNode()));
	//simVis::Viewpoint vp("current", 116.4, 39.5, 0.0, 0.0, -90.0, 10000000);
	//viewer3d_2->setViewpoint(vp);

	//targetGroup_3D_2 = new osg::Group();

	//viewer3d_2->getSceneManager()->getMapNode()->addChild(targetGroup_3D_2);
	//lod_3d_2 = new osg::LOD();
	//targetGroup_3D_2->addChild(lod_3d_2);
	//GLUtils::setGlobalDefaults(targetGroup_3D_2->getOrCreateStateSet());
}

//初始化2D场景
void gsView::make2dMap()
{
	//viewMan_3d = new simVis::ViewManager();
	Map* map = new Map();
	map->setProfile(Profile::create("spherical-mercator"));
	//map->setProfile(Profile::cr)
	osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	cacheOptions.rootPath() = "cache2d";
	//map->removeLayer(map->getLayerAt(0));
	map->setCache(osgEarth::CacheFactory::create(cacheOptions));
#if 0
	MBTilesImageLayer* osm = new MBTilesImageLayer();
	osm->setName("Whole Earth");
	osm->setURL("resources/lowResEarth.mbtiles");
	//osm->setProfile(Profile::create("spherical-mercator"));
#endif

#if 1
	// add a semi-transparent XYZ layer:
	XYZImageLayer* osm = new XYZImageLayer();
	osm->setName("osm");
	osm->setURL("http://[abc].tile.openstreetmap.org/{z}/{x}/{y}.png");
	osm->setProfile(Profile::create("spherical-mercator"));
#endif

#if 0
	// add a semi-transparent XYZ layer:
	XYZImageLayer* osm = new XYZImageLayer();
	osm->setName("osm");
	osm->setURL("http://[abc].tile.openstreetmap.org/{z}/{x}/{y}.png");
	osm->setProfile(Profile::create("spherical-mercator"));
#endif

#if 0
	ArcGISServerImageLayer* osm = new ArcGISServerImageLayer();
	//MBTilesImageLayer* osm = new MBTilesImageLayer();
	osm->setName("osm");
	//此处在内网环境下无法访问url会造成初始化时间过长
	//PS：需要考虑如何在内网环境下加载二维影像 - Note by gkk 20220626
	osm->setURL("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/");
	osm->setProfile(Profile::create("spherical-mercator"));
#endif

	map->addLayer(osm);

	TerrainOptions terrainOptions;
	terrainOptions.rangeMode() = osg::LOD::PIXEL_SIZE_ON_SCREEN;

	MapNode::Options mapNodeOptions;
	mapNodeOptions.terrain() = terrainOptions;

	MapNode* mapNode = new MapNode(map, mapNodeOptions);
	mapNode->setEnableLighting(false);
	//float elevation = 0.0;
	//mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), lon, la, &elevation);
	//mapNode->getTerrain()->getHeight(mapNode->getMapSRS())

	//二维加载矢量地图
	//GDALOptions basemapOpt;
	//basemapOpt.url() = "world.tif";
	//osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	//cacheOptions.rootPath() = "cache1";
	//// map->removeLayer(map->getLayerAt(0));
	//map->setCache(osgEarth::CacheFactory::create(cacheOptions));
	
	viewer2d = new osgViewer::View();

	viewer2dNode = mapNode;
	// Note that the logarithmic depth buffer is not installed

	// disable the default ESC-to-quit event:
	//viewMan_2d->getViewer()->setKeyEventSetsDone(0);
	//viewMan_2d->getViewer()->setQuitEventSetsDone(false);

	//sceneMan_2d = new simVis::SceneManager();
	//sceneMan_2d->setMapNode(mapNode);
	//sceneMan_2d->getMap()->setProfile(sceneMan_3d->getMap()->getProfile());
	//sceneMan_2d->getMap()->setProfile()
	//viewer2d->setNavigationMode(simVis::NAVMODE_GIS);
	//viewer2d->setUseOverheadClamping(true);
	//viewer2d->setNavigationMode(simVis::NAVMODE_GIS);
	mainip2d = new osgEarth::EarthManipulator();
	mainip2d->getSettings()->setMinMaxPitch(-90, -30);
	//mainip2d->getSettings()->setMinMaxDistance(0, 17000000);
	//mainip2d->getSettings()->setMaxOffset(60.0, 70.0);
	//mainip2d->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ActionType::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP);
	//mainip2d->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ActionType::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_DOWN);
	mainip2d->getSettings()->bindMouse(EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
	mainip2d->getSettings()->setThrowingEnabled(true);
	mainip2d->getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP);
	mainip2d->getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_DOWN);
	viewer2d->setCameraManipulator(mainip2d);
	viewer2d->addEventHandler(new osgViewer::LODScaleHandler);
	
	viewer2d->getCamera()->setClearColor(osg::Vec4f(0.53f, 0.81f, 0.92f, 0.2f));
	viewer2d->setUpViewInWindow(100, 100, 1600, 900, 0u);
	viewer2d->setSceneData(viewer2dNode);
	//viewer2d->setSceneManager(sceneMan_2d.get());
	//viewMan_3d->addView(viewer2d);
	//viewMan_2d->addView(viewer2d);

	//viewer2d->installDebugHandlers();
	viewer2d->getCamera()->setNearFarRatio(0.00002);
	viewer2d->getCamera()->setViewport(0, 0, 400, 400);
	viewer2d->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);
	viewer2d->getCamera()->setProjectionMatrixAsOrtho2D(MERC_MINX, MERC_MAXX, MERC_MINY, MERC_MAXY);

	//osgEarth::Viewpoint vp("current2d", 101.84, 35.65, 1000, 0.0, -90.0, 4000000);
	osgEarth::Viewpoint vp("current2d", 114.28, 31.12, 1000, 0.0, -90.0, 600000);
	mainip2d->setViewpoint(vp);
	group2d = new osg::Group();
	viewer2dNode->addChild(group2d);

	lod_2d = new osg::LOD();
	group2d->addChild(lod_2d);

}

void gsView::clearGroudObjects()
{
	targetGroup_3D->removeChildren(0,targetGroup_3D->getNumChildren());
	lod = new osg::LOD();
}

void gsView::config3DMap()
{
	Map* map1 = viewer3dNode->getMap();
	//Map* map2 = sceneMan_2d->getMap();
	osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	cacheOptions.rootPath() = "cache";

	/*设置缓存*/
	map1->setCache(osgEarth::CacheFactory::create(cacheOptions));
	map1->clear();
	//map2->clear();
}

void gsView::configMaps(zMapEntity* mapentity)
{
	Map* map1 = viewer3dNode->getMap();
	Map* map2 = viewer2dNode->getMap();
	//FileSystemCacheOptions cacheOptions;
	//cacheOptions.rootPath() = "cache";
	/*设置缓存*/
	//map1->setCache(osgEarth::CacheFactory::create(cacheOptions));
	map1->clear();
	map2->clear();
	if (mapentity->selected == 0)
	{
		//case 0:

		ArcGISServerImageLayer* layer = new ArcGISServerImageLayer();
		layer->setName("simdis.imagery");
		layer->setURL("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/");
		//map->addLayer(layer);
		map1->addLayer(layer);
		//加载本地图层
		//addLayers2();
		XYZImageLayer* osm = new XYZImageLayer();
		osm->setName("osm");
		osm->setURL("http://[abc].tile.openstreetmap.org/{z}/{x}/{y}.png");
		osm->setProfile(Profile::create("spherical-mercator"));
		map2->addLayer(osm);

		TMSElevationLayer* elevation = new TMSElevationLayer();
		elevation->setURL("http://data.marsgis.cn/terrain/");
		//map->addLayer(elevation);
		map1->addLayer(elevation);
		//	addLayers2();
		return;
	}
	
	else 
	{
		if (mapentity->image3dURL != "")
		{
			XYZImageLayer* osm = new XYZImageLayer();
			osm->setName("osm");
			osm->setURL(mapentity->image3dURL);
			osm->setProfile(Profile::create("spherical-mercator"));
			map1->addLayer(osm);
		}
		if (mapentity->image2dURL != "")
		{
			XYZImageLayer* osm = new XYZImageLayer();
			osm->setName("osm");
			osm->setURL(mapentity->image2dURL);
			osm->setProfile(Profile::create("spherical-mercator"));
			//map1->addLayer(osm);
			//map->addLayer(imagery);
			map2->addLayer(osm);
		}
		if (mapentity->elevationURL != "")
		{
			TMSElevationLayer* elevation = new TMSElevationLayer();
			elevation->setURL(mapentity->elevationURL);
			//map->addLayer(elevation);
			map1->addLayer(elevation);
		}
	//	addLayers2();
		return;
	}
}

//更新图层
void  gsView::updateLayer(zLayer* layer)
{
	Map* map1;
	if (!layer->type)
		map1 = viewer3dNode->getMap();
	//else
	//	map1 = sceneMan_2d->getMap();
	
	osgEarth::Layer* layer1 = map1->getLayerByName(layer->name);
	if (layer1)
	{
		if (!layer->layerType)
		{
			//map1->beginUpdate();
			layer1->close();
			WMSImageLayer* wms = dynamic_cast<WMSImageLayer*>(layer1);
			wms->setURL(layer->filePath);
			wms->setLayers(layer->layers);
			if(layer->showing)
				wms->open();
			return;
		}
		if (layer->layerType == 1)
		{
			layer1->close();
			GDALImageLayer* wms = dynamic_cast<GDALImageLayer*>(layer1);
			wms->setURL(layer->filePath);
			//wms->setLayers(layer->layers);
			if (layer->showing)
				wms->open();
			return;
		}
		if (layer->layerType == 2)
		{
			layer1->close();
			GDALElevationLayer* wms = dynamic_cast<GDALElevationLayer*>(layer1);
			wms->setURL(layer->filePath);
			//wms->setLayers(layer->layers);
			if (layer->showing)
				wms->open();
			return;
		}
		if (layer->layerType == 3)
		{
			//layer1->close();
			//ModelLayer* wms = dynamic_cast<ModelLayer*>(layer1);
			//map1->removeLayer(wms);
			this->removeSceneModel(layer->name);
			addLayer(layer);
			//if (layer->showing)
			//	wms->open();
			return;
		}
		if (layer->layerType == 4)
		{
			layer1->close();
			osgEarth::Contrib::ThreeDTilesLayer* wms = dynamic_cast<osgEarth::Contrib::ThreeDTilesLayer*>(layer1);
			wms->setURL(layer->filePath);
			//wms->setLayers(layer->layers);
			if (layer->showing)
				wms->open();
			return;
		}
	}
	else
	{
		addLayer(layer);
	}
}

void gsView::addLodGroup()
{
	osg::Group* gGive = new osg::Group();
	osg::Group* gGqx = new osg::Group();
	if (gGive != nullptr)
	{
		this->lod->addChild(gGive, 0, 1e5);
	}
	if (gGqx != nullptr)
	{
		this->lod->addChild(gGqx, 0, 1e5);
	}
}

void gsView::addLayer(zLayer* layer)
{
	Map* map;
	if (layer->type == 0)
	{
		map = viewer3dNode->getMap();
	}
	else
	{
		map = viewer2dNode->getMap();
	}
	//wms
	if (layer->layerType == 0)
	{
		WMSImageLayer* wms = new WMSImageLayer();
		wms->setURL(layer->filePath);
		wms->setFormat("png");
		wms->setLayers(layer->layers);
		wms->setName(layer->name);
		wms->setSRS("EPSG:4326");
		wms->setTransparent(true);

		map->addLayer(wms);
	}
	//正射影像图层
	else if (layer->layerType == 1)
	{
		GDALImageLayer* comp1 = new GDALImageLayer();
		comp1->setURL(layer->filePath);
		comp1->setName(layer->name);
		osgEarth::CompositeImageLayer* compImage = new osgEarth::CompositeImageLayer();
		compImage->addLayer(comp1);
		if(layer->layers!="")
		{ 
			std::vector<std::string> vSrs = CSysConst::vStringSplit(layer->layers, ",");
			for(int i=0;i<vSrs.size();i++)
			{ 
				GDALImageLayer* comp2 = new GDALImageLayer();
				comp2->setURL(vSrs[i]);
				compImage->addLayer(comp2);
			}
		}
		//comp1->setProfile(Profile::create("global-geodetic"));
		//comp1->setProfile(Profile::create("spherical-mercator"));
		
		map->addLayer(compImage);
	}
	//dem图层
	else if (layer->layerType == 2)
	{
		GDALElevationLayer* elev1 = new GDALElevationLayer();
		elev1->setURL(layer->filePath);
		elev1->setName(layer->name);
		map->addLayer(elev1);
	}
	//倾斜模型
	else if (layer->layerType == 3)
	{
		//osg::Group* qinxiegroup = sceneMan_3d->getOrCreateAttachPoint("qinxie");
		//QinxieModelLoader* loader = new QinxieModelLoader(map1, sceneMan_3d->getMapNode(), layer->filePath);
		//loader->ReadMetaInfoFromLocal();
		//loader->setLoadPosition(84.9, 39.63, 1200);
		std::vector<std::string> vSrs = CSysConst::vStringSplit(layer->layers, ",");
		osg::Vec3d pp(atof(vSrs[0].c_str()), atof(vSrs[1].c_str()), atof(vSrs[2].c_str()));
		osg::Vec3d pp2(atof(vSrs[3].c_str()), atof(vSrs[4].c_str()), atof(vSrs[5].c_str()));
		osg::Vec3d pp3(atof(vSrs[6].c_str()), atof(vSrs[7].c_str()), atof(vSrs[8].c_str()));
		this->loadSceneModel(layer->filePath, layer->name,pp,pp2,pp3);
	}
	//3dtiles
	else if (layer->layerType == 4)
	{
		osgEarth::Contrib::ThreeDTilesLayer *threeLayer = new osgEarth::Contrib::ThreeDTilesLayer;
		threeLayer->setURL(layer->filePath);
		
		map->addLayer(threeLayer);
	}
	//shp路网矢量文件
	else if (layer->layerType == 5)
	{
		//接下来，我们添加一个图层以提供要素数据。
		osgEarth::OGRFeatureSource* features = new osgEarth::OGRFeatureSource();
		features->setURL(layer->filePath);
		features->setName(layer->name);
		map->addLayer(features);

		//定义要素数据的样式。由于我们将渲染矢量作为线，配置线符号化器：
		osgEarth::Style style;

		//可见性
		osgEarth::RenderSymbol* rs = style.getOrCreate<osgEarth::RenderSymbol>();
		rs->depthTest() = false;
#if 0
		rs->depthOffset()->enabled() = true;
		rs->depthOffset()->automatic() = true;
#endif

		//贴地设置
		osgEarth::AltitudeSymbol* alt = style.getOrCreate<osgEarth::AltitudeSymbol>();
		alt->clamping() = alt->CLAMP_TO_TERRAIN;
		alt->technique() = alt->TECHNIQUE_DRAPE;

		osgEarth::LineSymbol* ls = style.getOrCreateSymbol<osgEarth::LineSymbol>();
		ls->stroke()->color() = osgEarth::Color::Yellow;
		ls->stroke()->width() = 3.0f;
		ls->tessellationSize()->set(100, osgEarth::Units::KILOMETERS);
#if 0
		ls->stroke()->smooth() = true;
		//ls->stroke()->stipple() = 255;
		ls->tessellation() = 5;
#endif

		//osgEarth::PolygonSymbol * polygonsymol = style.getOrCreateSymbol<osgEarth::PolygonSymbol>();
		//polygonsymol->fill()->color() = osgEarth::Color(152.0f / 255, 251.0f / 255, 152.0f / 255, 0.8f); //238 230 133
		//polygonsymol->outline() = true;

		//将要素的路径添加到图层里
		osgEarth::FeatureImageLayer* layer = new osgEarth::FeatureImageLayer();
		layer->setFeatureSource(features);
		//将style风格加载到图层中
		osgEarth::StyleSheet* sheet = new osgEarth::StyleSheet();
		sheet->addStyle(style);
		layer->setStyleSheet(sheet);
		map->addLayer(layer);

#if 0
		osgEarth::AttributeTable
		//const osgEarth::Geometry* pg = features->getGeometry();
		qDebug() << features->getFeatureCount();
		const osgEarth::Geometry* pg = features->getFeature(0)->getGeometry();
			//FeatureID

		if (pg)
		{
			osgEarth::Vec3dVector vec3d = pg->asVector();

			for each (osg::Vec3d var in vec3d)
			{
				qDebug() << QString::number(var.x() ,'f')<< QString::number(var.y(), 'f') << var.z();
			}
			qDebug() << vec3d.size();
		}
#endif
		
	}
	//wmts server
	else if (layer->layerType == 6)
	{
		const Profile* profile = Profile::create("wgs84", "egm96");
		map->setProfile(profile);

		osgEarth::URIContext context;
		context.addHeader("Accept", "image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8");
		context.addHeader("Accept-Encoding", "gzip, deflate, br");
		context.addHeader("Accept-Language", "zh-CN,zh;q=0.9");
		context.addHeader("Connection", "keep-alive");
		context.addHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Mobile Safari/537.36");
		
		////注意URL的格式可以通过抓包来确定 - add by gkk 20220527
		//osgEarth::URI uri("http://192.168.18.1:8080/geoserver/gwc/service/wmts?layer=wp630:hp16&style=&tilematrixset=EPSG:4326&Service=WMTS&Request=GetTile&Version=1.0.0&Format=image%2Fpng&TileMatrix=EPSG:4326:{z}&TileCol={x}&TileRow={y}",
		//	context);
		//osgEarth::URI uri("http://172.16.3.141:8080/geoserver/gwc/service/wmts?layer=wp630:hp16&style=&tilematrixset=EPSG:4326&Service=WMTS&Request=GetTile&Version=1.0.0&Format=image/png&TileMatrix=EPSG:4326:{z}&TileCol={x}&TileRow={y}",
		//	context);

		osgEarth::URI uri(layer->filePath, context);

		XYZImageLayer* tms = new XYZImageLayer();
		tms->setURL(uri);
		tms->setProfile(profile);
		tms->setName(layer->name);
		map->addLayer(tms);
	}
	//离线影像瓦片数据，必须要先用nginx发布成服务
	else if (layer->layerType == 7)
	{
		//MyGISServerImageLayer* localTile = new MyGISServerImageLayer();
		//osgEarth::URI uri("C:/Dev/test/vs2015/TestWaPian/TestWaPian/data/hpq");
		//localTile->setURL(uri);
		//map->addLayer(localTile);
		//osgEarth::URI uri(layer->filePath, context);

		osgEarth::URI uri("http://localhost:8081/hpq/{z}/{x}/{y}.jpg");
		XYZImageLayer* tms = new XYZImageLayer();
		tms->setURL(uri);
		tms->setProfile(Profile::create("spherical-mercator"));
		tms->setName(layer->name);
		tms->setMinLevel(7);
		map->addLayer(tms);
	}
}

void gsView::loadSceneModel(string modelpath, string name,osg::Vec3d &p,osg::Vec3d &scale, osg::Vec3d &rph)
{
	QString path = QString::fromLocal8Bit(modelpath.c_str());

	osg::Group* gg = new osg::Group();
	
	gg->setName(name);

	osg::MatrixTransform* pat = new osg::MatrixTransform();
	pat->setName(name);

	//pat->addChild(gg);
    pat->setMatrix(Matrix::scale(scale.x(), scale.y(), scale.z())
			*Matrix::rotate(osg::inDegrees(rph.x()), osg::Z_AXIS)
			*Matrix::rotate(osg::inDegrees(rph.y()), osg::X_AXIS)
			*Matrix::rotate(osg::inDegrees(rph.z()), osg::Y_AXIS));

	if(path.lastIndexOf("/") != (path.length() - 1))
	{ 
		//加载模型
		osg::ref_ptr<osg::Node> model = osgDB::readRefNodeFile(modelpath);
		if (model == nullptr) {
			return;
		}
		model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//model->setName(name);
		/*model->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		model->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
		model->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);*/
		osgEarth::GeoPoint point(SpatialReference::create("wgs84"), p.x(), p.y(), p.z(), ALTMODE_ABSOLUTE);
		//osg::Matrix m;
		//point.createLocalToWorld(m);
		//osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform(m);
		//m.scale(osg::Vec3(15000, 15000, 15000));
		//mt->addChild(model.get());
		osg::ref_ptr<osgEarth::GeoTransform> xform = new osgEarth::GeoTransform();

		xform->setTerrain(viewer3dNode->getTerrain());

		//xform->addChild(model);
		//osgEarth::GeoPoint point(sceneMan_3d->getMapNode()->getMapSRS()->getGeographicSRS(), mVecPos.x(), mVecPos.y(), mVecPos.z());                 //使用绝对高，正高
		xform->setPosition(point);
	
		//sc->getShadowCastingGroup()->addChild(mt.get());
		//lod->addChild(xform.get(),0,1e6);
		//mRoot->getParent(0)->addChild(sc);
		osgEarth::Registry::shaderGenerator().run(model.get());
		pat->addChild(model);
			xform->addChild(pat);
			gg->addChild(xform);
			//if (path.contains(".ive")) {
			//	this->lod->getChild(0)->asGroup()->addChild(gg);
			//}
			//else if (path.contains(".osgb"))
			//{
			//	this->lod->getChild(1)->asGroup()->addChild(gg);
			//}
			//this->lod->addChild(gg, 0, 1e5);
	}
	else
	{
		string rootFolder = modelpath+"data\\";
		osg::ref_ptr<osgEarth::GeoTransform> xform = new osgEarth::GeoTransform();

		xform->setTerrain(viewer3dNode->getTerrain());
		osgEarth::GeoPoint point(SpatialReference::create("wgs84"), p.x(), p.y(), p.z(), ALTMODE_ABSOLUTE);
		//osgEarth::GeoPoint point(mapNode->getMapSRS()->getGeographicSRS(), mVecPos.x(), mVecPos.y(), mVecPos.z());                 //使用绝对高，正高
		xform->setPosition(point);
		/*xform->getMatrix().scale(scale.x(), scale.y(), scale.z());
		xform->getMatrix().rotate(osg::inDegrees(rph.x()), osg::Z_AXIS);
		xform->getMatrix().rotate(osg::inDegrees(rph.y()), osg::X_AXIS);
		xform->getMatrix().rotate(osg::inDegrees(rph.z()), osg::Y_AXIS);*/
		//遍历每一个文件夹
		//	QStringList fileList;
		QDir dir(QString::fromLocal8Bit(rootFolder.c_str()));
		dir.setFilter(QDir::Dirs);
		dir.setSorting(QDir::Name);
		QStringList list = dir.entryList();
		for (int i = 0; i < list.size(); i++)
		{
			if (list[i].indexOf(".") < 0)
			{
				QDir dir2(QString::fromLocal8Bit(rootFolder.c_str()) + list[i] + "/");
				dir2.setFilter(QDir::Files);
				dir2.setSorting(QDir::Name);
				QStringList list2 = dir2.entryList();
				/*for(int j=0;j<list2.size();j++)
				{ */

				osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(rootFolder + list[i].toStdString() + "/" + list2[0].toStdString());
				//node->setName(name);
				if (node.valid())
				{
					osgEarth::Registry::shaderGenerator().run(node);
					//xform->addChild(node);
					//pat->addChild(node);
					pat->addChild(node);
					xform->addChild(pat);

				}
				/*}*/
				//break;
			}
		}
		//osg::ref_ptr<osgEarth::ModelLayer> modelLayer = new osgEarth::ModelLayer();
		//ModelLayer* layer = new ModelLayer();
		//xform->addChild(pat);
		gg->addChild(xform);
		//this->lod->getChild(1)->asGroup()->addChild(gg);
	//}
		//modelLayer->setName(name);
		//group->addLayer(modelLayer);
	}
	
	/*pat->getMatrix().scale(scale.x(), scale.y(), scale.z());
	pat->getMatrix().rotate(osg::inDegrees(rph.x()), osg::Z_AXIS);
	pat->getMatrix().rotate(osg::inDegrees(rph.y()), osg::X_AXIS);
	pat->getMatrix().rotate(osg::inDegrees(rph.z()), osg::Y_AXIS);*/
	//targetGroup_3D->addChild(gg);

	this->lod->addChild(gg, 0, 1e5);
}

osg::Matrix& gsView:: getModelMatrix(string name)
{
	osg::Matrix mat1;
	for (int i = 0; i < lod->getNumChildren(); i++)
	{
		osg::Node* node = lod->getChild(i);
		if (node->getName() == name)
		{
			osg::Group* gg = (osg::Group*)node;
			osg::MatrixTransform* pat = (osg::MatrixTransform*)gg->getChild(0);
			mat1 = pat->getMatrix();
		}
	}
	return mat1;
}

void gsView::set2DViewpoint(osgEarth::Viewpoint vp)
{
	////viewer2d->setCameraManipulator
	////viewer2d->setViewpoint(vp);
	//osgEarth::EarthManipulator *mainip2d1 = new osgEarth::EarthManipulator();
	//mainip2d1->getSettings()->setMinMaxPitch(-90, -15);
	//mainip2d1->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ActionType::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP);
	//mainip2d1->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ActionType::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_DOWN);
	//mainip2d1->getSettings()->setThrowingEnabled(true);
	//viewer2d->setCameraManipulator(mainip2d1);

	//viewer2d->getCamera()->setClearColor(osg::Vec4f(0.53f, 0.81f, 0.92f, 0.2f));
	////viewer2d->setUpViewInWindow(100, 100, 1600, 900, 0u);

	//viewer2d->setSceneManager(sceneMan_2d.get());
	//viewMan_3d->addView(viewer2d);
	//viewer2d->installDebugHandlers();
	//viewer2d->getCamera()->setNearFarRatio(0.00002);
	//viewer2d->getCamera()->setViewport(0, 0, 400, 400);
	//viewer2d->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);
	//viewer2d->getCamera()->setProjectionMatrixAsOrtho2D(MERC_MINX, MERC_MAXX, MERC_MINY, MERC_MAXY);
	

	mainip2d->setViewpoint(vp);
}

void gsView::removeSceneModel(string name)
{
	for (int i = 0; i < lod->getNumChildren(); i++)
	{
		osg::Node* node = lod->getChild(i);
		if (node->getName() == name)
			lod->removeChild(node);
	}
}

/*************************************
 * Brief     : 移除图层
 * Parameter : zLayer * p_layer，需要移除的图层
 * Author    : GKK 2022/04/21
*************************************/
void gsView::removeLayer(zLayer* p_layer)
{
	Map* map;
	if (p_layer->type == 0)
	{
		map = viewer3dNode->getMap();
	}
	else
	{
		map = viewer2dNode->getMap();
	}

	if (map)
	{
		//Layer* getLayerByName(const std::string& name)
		Layer* pLayer = map->getLayerByName(p_layer->name);
		map->removeLayer(pLayer);
	}
}

/*************************************
 * Brief     : 移除所有图层
 * Parameter : osg::ref_ptr<simVis::SceneManager> scenMan，移除哪个场景下的图层
 * Author    : GKK 2022/04/21
*************************************/
void gsView::removeAllLayers(MapNode* scenMan)
{
	Map* map = scenMan->getMap();
	if (map)
	{
		map->clear();
	}
}

/**
 * Brief     : 返回是否包含第二个三维场景（小场景）
 * Returns   : bool，true表示创建了第二个三维场景
 * Author    : GKK 2022/09/22
 */
bool gsView::has3DScene_2()
{
	return viewer3d_2 == nullptr ? false : true;
}

osgViewer::View* gsView::get3DScene_2()
{
	if (nullptr == viewer3d_2)
	{
		initEarth2();
	}
	return viewer3d_2;
}

void gsView::enable2D3DLinkage()
{
	if (nullptr == mainip3d || nullptr == mainip2d)
	{
		return;
	}
	if (nullptr == dargHandle_2d)
	{
		dargHandle_2d = new Darg2Dto3DHandle(mainip3d);
	}
	if (nullptr == dargHandle_3d)
	{
		dargHandle_3d = new Darg2Dto3DHandle(mainip2d);
	}

	viewer2d->addEventHandler(dargHandle_2d);
	viewer3d->addEventHandler(dargHandle_3d);
}

void gsView::disable2D3DLinkage()
{
	if (nullptr != dargHandle_2d && nullptr != dargHandle_3d)
	{
		viewer2d->removeEventHandler(dargHandle_2d);
		viewer3d->removeEventHandler(dargHandle_3d);
	}
}

