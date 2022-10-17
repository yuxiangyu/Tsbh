#include "MeasureRadialAnalysis.h"
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>
#include <osg/LineWidth>


MeasureRadialAnalysis::MeasureRadialAnalysis(osg::Group* group, MapNode* node):TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	DrapeableNode = new osg::Group;
	mapNode->open();
//	DrapeableNode->setMapNode(mapNode);

	mFeature = NULL;


	m_pFeature = NULL;
	m_pFeatureNode = NULL;
	m_pStippleFeature = NULL;
	m_pStippleFeatureNode = NULL;

	m_vecPoint.clear();
}


MeasureRadialAnalysis::~MeasureRadialAnalysis()
{
	//mRoot->removeChild(mFeatureNode);
}

void  MeasureRadialAnalysis::onLeftButton(GeoPoint* Point)
{
	/*if (m_vecPoint.size() == 2)
	{
		osg::Vec3d point(Point->x(), Point->y(), Point->alt());
		m_vecPoint[1] = point;
		buildModel();
	}
	else*/ if(m_vecPoint.size()<2)
	{
		m_vecPoint.push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		if (m_vecPoint.size() == 2)
		{
			buildModel();
		}
	}
	
		

}

void  MeasureRadialAnalysis::onDoubleClick(GeoPoint* Point)
{

	onLeftButton(Point);
	this->m_bFinish = true;
}

void  MeasureRadialAnalysis::onMouseMove(GeoPoint* Point)
{

}

void MeasureRadialAnalysis::buildModel()
{
	const SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
	if (m_tetheredRadial != nullptr)
	{
		DrapeableNode->removeChildren(0, DrapeableNode->getNumChildren());
	}
	

	if (m_vecPoint.size() > 1)
	{
		//osgEarth::Contrib::RadialLineOfSightNode* A_rlos_tmpd = new osgEarth::Contrib::RadialLineOfSightNode(mapNode);
		//A_rlos_tmpd->setFill(true);
		osg::Vec3d start = m_vecPoint[0];

		osgEarth::GeoPoint startpos(geoSRS, start.x(), start.y(), start.z() < 0 ? 0 : start.z(), ALTMODE_ABSOLUTE);
		osg::Vec3d startWpos;
		startpos.toWorld(startWpos);

		osg::Vec3d end = m_vecPoint[1];

		osgEarth::GeoPoint endpos(geoSRS, end.x(), end.y(), end.z() < 0 ? 0 : end.z(), ALTMODE_ABSOLUTE);
		osg::Vec3d endWpos;
		endpos.toWorld(endWpos);
	



		double rad = (endWpos - startWpos).length()*1.2;

	
		m_tetheredRadial = new RadialLineOfSightNode2(mapNode);
		m_tetheredRadial->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		//tetheredRadial->setRadius(5000);
		
		m_tetheredRadial->setCenter(startpos);
		m_tetheredRadial->setRadius(rad);
		//m_tetheredRadial->setFill(false);
		//m_tetheredRadial->setGoodColor(osg::Vec4(0, 1, 0, 0.3));
		//m_tetheredRadial->setBadColor(osg::Vec4(1, 0, 0, 0.3));
		//m_tetheredRadial->setNumSpokes(100);
		//m_tetheredRadial->setDisplayMode(osgEarth:Contrib::LineOfSight::DisplayMode)
		//RadialLineOfSightEditor2* radialEditor = new RadialLineOfSightEditor2(m_tetheredRadial);
		DrapeableNode->addChild(m_tetheredRadial);
		mRoot->addChild(DrapeableNode);
	}

	//	osgEarth::GeoPoint startpos(geoSRS, start.x(), start.y(), start.z()<0 ? 0 : start.z(), ALTMODE_ABSOLUTE);
	//	osg::Vec3d startWpos;
	//	startpos.toWorld(startWpos);

	//	osg::Vec3d end = m_vecPoint[1];

	//	osgEarth::GeoPoint endpos(geoSRS, end.x(), end.y(), end.z()<0 ? 0 : end.z(), ALTMODE_ABSOLUTE);
	//	osg::Vec3d endWpos;
	//	endpos.toWorld(endWpos);

	//	A_rlos_tmpd->setCenter(startpos);
	//	//A_rlos_tmpd->setNumSpokes(20);
	//	//A_rlos_tmpd->setFill(true);
	//	//A_rlos_tmpd->setRadius(1.0);
	//	A_rlos_tmpd->setGoodColor(osg::Vec4(0, 1, 0, 0.5));
	//	A_rlos_tmpd->setBadColor(osg::Vec4(1, 0, 0,0.3));
	//	
	//	A_rlos_tmpd->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);


	//	double rad = (endWpos - startWpos).length();

	//	A_rlos_tmpd->setRadius(rad);
	//	osg::ref_ptr<osg::StateSet> stateset = A_rlos_tmpd->getOrCreateStateSet();
	//	osg::ref_ptr<osg::LineWidth> lineWid = new osg::LineWidth(5);

	//	stateset->setAttribute(lineWid);

	//	mRoot->addChild(A_rlos_tmpd);

	//}

}

osg::Node* MeasureRadialAnalysis::createLabel()
{
	Style style;
	
	//style.getOrCreate<BBoxSymbol>()->geom() = BBoxSymbol::BboxGeom::GEOM_BOX_ORIENTED;
	style.getOrCreate<BBoxSymbol>()->border()->width() = 2.0;
	style.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 0.0, 1.0, 0.3);
	style.getOrCreate<BBoxSymbol>()->border()->color() = Color(1.0, 1.0, 1.0, 1.0);
	style.getOrCreate<TextSymbol>()->size() = 18.0f;
	style.getOrCreate<TextSymbol>()->fill()->color() = Color(1.0f, 0.0f, 1.0f, 1.0f);
	style.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	style.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	
	//style.getOrCreate<BBoxSymbol>()->border()->widthUnits()= Units::PIXELS;
	osg::Vec3d point;
	point.x() = (lineString->at(lineString->size() - 2).x() + lineString->at(lineString->size() - 3).x()) / 2;
	point.y() = (lineString->at(lineString->size() - 2).y() + lineString->at(lineString->size() - 3).y()) / 2;
	point.z() = (lineString->at(lineString->size() - 2).z() + lineString->at(lineString->size() - 3).z()) / 2;;
	GeoPoint currentPoint(mapNode->getMapSRS()->getGeographicSRS(), lineString->at(lineString->size() - 2));
	GeoPoint lastGeoPoint(mapNode->getMapSRS()->getGeographicSRS(), lineString->at(lineString->size() - 3));
	auto distance = currentPoint.distanceTo(lastGeoPoint);
	std::stringstream stream;
	stream.precision(2);
	stream.setf(std::ios::fixed);
	if (distance > 1000)
		stream << distance / 1e3 << u8"วงรื";
	else
		stream << distance << u8"รื";
	LabelNode* label= new LabelNode( stream.str(), style);
	label->setPosition(GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), point.x(), point.y(), point.z()));
	
	return label;
}

void MeasureRadialAnalysis::renderCircleNode(GeoPoint* point)
{
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f);
	osg::Geode* geode = new osg::Geode();
	osg::ShapeDrawable* _shapeDrawable = new osg::ShapeDrawable(shape);
	_shapeDrawable->setDataVariance(osg::Object::DYNAMIC);
	_shapeDrawable->setColor(Color(0, 0, 1, 1));
	geode->addDrawable(_shapeDrawable);

	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::Vec3d worldP;
	point->toWorld(worldP);

	//mapPoint.createLocalToWorld(matrix);

	osg::MatrixTransform* _scaler = new osg::MatrixTransform;
	_scaler->setMatrix(osg::Matrixd::scale(5, 5, 5));
	_scaler->addChild(geode);

	osg::AutoTransform* at = new osg::AutoTransform;
	at->setAutoScaleToScreen(true);
	at->addChild(_scaler);

	osg::MatrixTransform* _transfer = new osg::MatrixTransform;
	_transfer->setMatrix(osg::Matrixd::translate(worldP.x(), worldP.y(), worldP.z()));
	_transfer->addChild(at);
	mRoot->addChild(_transfer);
}
