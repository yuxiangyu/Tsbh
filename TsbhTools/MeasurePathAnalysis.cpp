#include "MeasurePathAnalysis.h"
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>
#include <osg/LineWidth>
#include <osgEarth/GLUtils>
#include <osgEarth/TerrainProfile>
#include <osg/BlendFunc>
#include <osg/Point>
#include "SignageNode.h"
//#include "TerrainProfileGraph.h"
#include <QDebug>

MeasurePathAnalysis::MeasurePathAnalysis(osg::Group* group, MapNode* node):TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	m_vecPoint.clear();
	lineString = new osgEarth::LineString();
}


MeasurePathAnalysis::~MeasurePathAnalysis()
{
	
}

void  MeasurePathAnalysis::onLeftButton(GeoPoint* Point)
{
	if (m_vecPoint.size() == 0)
	{
		lineString->clear();
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		lineString->push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		mFeature = new Feature(lineString, mapNode->getMapSRS()->getGeographicSRS());
		mFeature->geoInterp() = GEOINTERP_RHUMB_LINE;
		Style style;
		AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
		alt->technique() = AltitudeSymbol::TECHNIQUE_GPU;
		alt->verticalOffset() = 0.1;
		alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		RenderSymbol* render = style.getOrCreate<RenderSymbol>();
		render->minAlpha() = 0.15f;
		LineSymbol* ls = style.getOrCreate<LineSymbol>();

		ls->stroke()->color() = Color::Blue;

		ls->stroke()->width() = 2.0f;
		ls->stroke()->widthUnits() = Units::PIXELS;
		ls->tessellation() = 20;

		mFeatureNode = new FeatureNode(mFeature, style);
		mRoot->addChild(mFeatureNode);
	}

	if (m_vecPoint.size() == 2)
	{
		m_vecPoint[1] = Point;
	}
	else
	{
		m_vecPoint.push_back(Point);
		if (m_vecPoint.size() == 2)
		{
			buildModel();
		}
	}

}

void  MeasurePathAnalysis::onDoubleClick(GeoPoint* Point)
{

	onLeftButton(Point);
	this->m_bFinish = true;
}

void  MeasurePathAnalysis::onMouseMove(GeoPoint* Point)
{
	if (m_vecPoint.size() < 1)
		return;
	
	lineString->at(lineString->size() - 1).x() = Point->x();
	lineString->at(lineString->size() - 1).y() = Point->y();
	lineString->at(lineString->size() - 1).z() = Point->z();
	mFeatureNode->dirty();
	
}

void MeasurePathAnalysis::buildModel()
{
	if (m_vecPoint.size() < 2)
		return;
	GeoPoint* startv3d = m_vecPoint.at(0);
	//startv3d->altitudeMode() = ALTMODE_RELATIVE;
	GeoPoint* endv3d = m_vecPoint.at(1);
	/*double graphWidth = 2000;
	double graphHeight = 1000;


	osg::ref_ptr< osgEarth::Contrib::TerrainProfileCalculator > calculator = new osgEarth::Contrib::TerrainProfileCalculator(mapNode,
		GeoPoint(mapNode->getMapSRS(), *startv3d),
		GeoPoint(mapNode->getMapSRS(), *endv3d)
	);

	osgEarth::Contrib::TerrainProfileGraph* profileNode = new osgEarth::Contrib::TerrainProfileGraph(calculator.get(), graphWidth, graphHeight);
	
*/
	
	

	osg::Vec3d startWpos;
	startv3d->toWorld(startWpos);
	renderCircleNode(startWpos,QString::fromLocal8Bit("起点"));

	osg::Vec3d endpos;
	endv3d->toWorld(endpos);
	renderCircleNode(endpos, QString::fromLocal8Bit("终点"));

	osg::ref_ptr<osg::Vec3Array> outPosArr = getInsPoints(mapNode, startWpos, endpos, 100);
	outPosArr->push_back(endpos);
	outPosArr->pop_back();
	osg::ref_ptr<osg::Vec3Array> posArr_ = new  osg::Vec3Array();
	posArr_->resize(outPosArr->size());

	double dMinHeight = 0.0, dMaxHeight = 0.0;
	double dMaxDistance = 0.0;
	osgEarth::GeoPoint insp1,insp2;
	osg::Vec3d p1, p2;
	osgEarth::GeoPoint midpos;
	for (size_t i = 0; i < outPosArr->size(); i++)
	{
		posArr_->at(i) = outPosArr->asVector()[i];
		insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), posArr_->at(i));
		qDebug() << insp2.x() << ":" << insp2.y() << ":" << insp2.z();
		if (i == 0)
		{
			dMinHeight = insp2.z();
			dMaxHeight = insp2.z();
		}
		if (dMinHeight > insp2.z())
			dMinHeight = insp2.z();
		if(dMaxHeight<insp2.z())
			dMaxHeight = insp2.z();
		if (i == 50)
			midpos = insp2;

		if(i>0)
		{
			insp1.toWorld(p1);
			insp2.toWorld(p2);
			dMaxDistance += (p2 - p1).length();
		}
		insp1 = insp2;
	}

	osg::Geode *edges = drawEdges(posArr_);

	mRoot->addChild(edges);


	//计算最小距离
	auto dMinDistance = (endpos - startWpos).length();
	
 	buildLabelModel(midpos, dMinDistance, dMaxDistance, dMinHeight, dMaxHeight);
	m_vecPoint.clear();
	mRoot->removeChild(mFeatureNode);
}

osg::ref_ptr<osg::Vec3Array> MeasurePathAnalysis::getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num)
{
	osg::ref_ptr<osg::Vec3Array> outPosArr1 = new osg::Vec3Array();
	outPosArr1->clear();
	osgEarth::GeoPoint insp2;

	outPosArr1->push_back(start);
	osg::Vec3 dir(end[0] - start[0], end[1] - start[1], end[2] - start[2]);
	for (int i = 0; i < num; i++)
	{
		osg::Vec3 p_start = start + dir * float(i) / num;
		p_start = p_start *(1 + 0.1);
		osg::Vec3 p_end = p_start *(1 - 0.1);

		osgUtil::LineSegmentIntersector::Intersections _intersections;
		osg::ref_ptr< osgUtil::LineSegmentIntersector > _lineSegmentIntersector = new osgUtil::LineSegmentIntersector(p_start, p_end);
		osgUtil::IntersectionVisitor _iv(_lineSegmentIntersector.get());
		mapNode->accept(_iv);

		_intersections = _lineSegmentIntersector->getIntersections();
		int _intersectionNumber = _intersections.size();
		if (_intersectionNumber != 0)
		{
			osgUtil::LineSegmentIntersector::Intersections::iterator hitr = _intersections.begin();
			osg::Vec3d insp = hitr->getWorldIntersectPoint();

			//insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), insp);
			////insp2.z() -= 500;
			//insp2.toWorld(insp);
			outPosArr1->push_back(insp);
		}
	}

	return  outPosArr1;
}

osg::Geode * MeasurePathAnalysis::drawEdges(osg::ref_ptr<osg::Vec3Array>  points)
{
	//创建保存几何信息的对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

	geom->setVertexArray(points.get());

	//为每个顶点指定一种颜色
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

	int nn = points->size();

	for (size_t i = 0; i < points->size(); i++)
		c->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));


	//如果没指定颜色则会变为黑色
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//line
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, points->size()));
	

	osg::ref_ptr<osg::Geometry> pointgeom = new osg::Geometry();

	pointgeom->setVertexArray(points.get());
	osg::ref_ptr<osg::Vec4Array> pc = new osg::Vec4Array();

	for (size_t i = 0; i < points->size(); i++)
		pc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));

	pointgeom->setColorArray(pc.get());
	pointgeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	pointgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, points->size()));


	/////geode
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.0), osg::StateAttribute::ON);


	osg::StateSet* stateset = geode->getOrCreateStateSet();

	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);

	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);




	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func      
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	geode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	//point

	osg::Point* pointSize = new osg::Point();
	pointSize->setSize(5.0);
	geode->getOrCreateStateSet()->setAttribute(pointSize, osg::StateAttribute::ON);


	//geode->addDrawable(pointgeom.get());
	geode->addDrawable(geom.get());


	return geode.release();

}

void MeasurePathAnalysis::renderCircleNode(osg::Vec3d worldP, QString lablel)
{
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f);
	osg::Geode* geode = new osg::Geode();
	osg::ShapeDrawable* _shapeDrawable = new osg::ShapeDrawable(shape);
	_shapeDrawable->setDataVariance(osg::Object::DYNAMIC);
	_shapeDrawable->setColor(Color(1, 0, 0, 1));
	geode->addDrawable(_shapeDrawable);

	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

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


	Style style;
	style.getOrCreate<BBoxSymbol>()->border()->width() = 2.0;
	style.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	style.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	style.getOrCreate<TextSymbol>()->size() = 18.0f;
	style.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	style.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	style.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;


	std::string text = lablel.toStdString();
	LabelNode* label = new LabelNode(text, style);
	label->setPosition(GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), worldP.x(), worldP.y(), worldP.z()));
	mRoot->addChild(label);
}

void MeasurePathAnalysis::buildLabelModel(GeoPoint position, double dMinDistance, double dMaxDistance, double dMinHeight, double dMaxHeight)
{

	SignageNode* mLabelNode;
	osgEarth::Style labelStyle;
	TextSymbol* textSymbol = labelStyle.getOrCreate<osgEarth::TextSymbol>();
	textSymbol->fill()->color() = osgEarth::Color::White;
	textSymbol->alignment() = osgEarth::TextSymbol::ALIGN_LEFT_CENTER;
	textSymbol->pixelOffset() = osg::Vec2s(2, 100);
	textSymbol->font() = "c:/windows/fonts/simhei.ttf";
	/*textSymbol->fill()->color() = Color("#034607");*/
	textSymbol->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;

	std::stringstream stream;
	stream.precision(2);
	stream.setf(std::ios::fixed);
	stream << "最大距离:" << dMaxDistance << "\n最小距离:" << dMinDistance << "\n最大高度:" << dMaxHeight << "\n最小高度"<< dMinHeight;

	mLabelNode = new SignageNode(mapNode, mRoot,osgEarth::GeoPoint(), stream.str(), labelStyle, SignageNode::Simple);
	mLabelNode->setDynamic(true);
	mLabelNode->setLabelType(1);
	mLabelNode->setStyle2(labelStyle);
	osgEarth::Registry::shaderGenerator().run(mLabelNode);
	mLabelNode->setPosition(position);
	mRoot->addChild(mLabelNode);
}

