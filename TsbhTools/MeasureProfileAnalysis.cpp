//////////////////////////////////////////////////////////////////////////
/// Copyright (C), 1998-2016, rytec Corporation. All rights reserved.
/// \brief      三维点位量算图形类

//////////////////////////////////////////////////////////////////////////

#include "MeasureProfileAnalysis.h"
#include <osgEarth/MapNode>
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Point>
#include "qdebug.h"
//顶点着色器
static const char* vertShader = {
	"varying vec4 color;\n"
	"void main(void)\n"
	"{\n"
	"color = gl_Vertex;\n"
	"gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;\n"
	"}\n"
};

//片元着色器
static const char* fragShader = {
	"varying vec4 color;\n"
	"void main(void)\n"
	"{\n"
	"    gl_FragColor = clamp(color,0.1,0.8);\n"
	"}\n"
};
MeasureProfileAnalysis::MeasureProfileAnalysis(osg::Group* group, MapNode* node) :TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	position = NULL;
	m_vecPoint.clear();
	isUpdateFeature = false;


}

MeasureProfileAnalysis::~MeasureProfileAnalysis()
{
}


//void MeasureProfileAnalysis::buildModel()
//{
//   // CRyGPoint* gPoint = dynamic_cast<CRyGPoint*>(mGeometry);
//	if (!position)
//		return;
//   
//
//        osgEarth::Style labelStyle;
//        TextSymbol* textSymbol = labelStyle.getOrCreate<osgEarth::TextSymbol>();
//        textSymbol->fill()->color() = osgEarth::Color::Yellow;
//        textSymbol->alignment() = osgEarth::TextSymbol::ALIGN_LEFT_CENTER;
//        textSymbol->pixelOffset() = osg::Vec2s(2, 100);
//        textSymbol->font() = "fonts/simhei.ttf";
//        textSymbol->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
//
//        BBoxSymbol* bBoxSymbol = labelStyle.getOrCreate<osgEarth::BBoxSymbol>();
//        bBoxSymbol->fill()->color() = osgEarth::Color(1.0, 1.0, 1.0, 0.3);
//        bBoxSymbol->border()->color() = osgEarth::Color(1.0, 1.0, 1.0, 0.8);
//		std::stringstream stream;
//		stream.precision(2);
//		stream.setf(std::ios::fixed);
//			stream << "经度:"<< position->x()<<"\n纬度:"<< position->y()<<"\n高度:"<< position->z()<<"\n";
//        
//        //std::string labelTextUtf8 = CRyStringUtil::SysWideToUTF8(labelText.c_str());
//        mLabelNode = new RichLabelNode(mapNode, osgEarth::GeoPoint(), stream.str(), labelStyle);
//        mLabelNode->setDynamic(true);
//
//        mRoot->addChild(mLabelNode);
//}

void  MeasureProfileAnalysis::onLeftButton(GeoPoint* Point)
{
	isUpdateFeature = true;
	renderCircleNode(Point);
	if (m_vecPoint.size() == 2)
	{
		osg::Vec3d point(Point->x(), Point->y(), Point->alt());
		m_vecPoint[1] = point;
		buildModel();
		//updateFeatureNode();
	}
	else
	{
		m_vecPoint.push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
		if (m_vecPoint.size() == 2)
		{
			buildModel();
			//updateFeatureNode();
		}
	}

	if (m_vecPoint.size() == 2)
	{
		isUpdateFeature = false;
		this->m_bFinish = true;
	}
}

void  MeasureProfileAnalysis::onDoubleClick(GeoPoint* Point)
{
	onLeftButton(Point);
	isUpdateFeature = false;
	this->m_bFinish = true;
}

void  MeasureProfileAnalysis::onMouseMove(GeoPoint* Point)
{
	

		if (m_vecPoint.size() == 2)
		{
			osg::Vec3d point(Point->x(), Point->y(), Point->alt());
			m_vecPoint[1] = point;
			//buildModel();
			if(isUpdateFeature)
			 updateFeatureNode();
		}
		else
		{
			m_vecPoint.push_back(osg::Vec3d(Point->x(), Point->y(), Point->alt()));
			if (m_vecPoint.size() == 2)
			{
				//buildModel();
				if(isUpdateFeature)
				updateFeatureNode();
			}
		}



}
void MeasureProfileAnalysis::renderCircleNode(GeoPoint* point)
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

//osg::Vec3d MeasureProfileAnalysis::getControlPoints()
//{
//	osg::Vec3d points;
//	osg::Vec3d* lineString = dynamic_cast<CRyLineString*>(getGeometryRef());
//	if (NULL == lineString)
//		return points;
//
//	points.resize(lineString->getNumPoints());
//	for (UINT32 i = 0; i < points.size(); i++)
//	{
//		double x = 0.0, y = 0.0, z = 0.0;
//		lineString->getPoint(i, x, y, z);
//		points[i] = CRyVec3D(x, y, z);
//	}
//
//	return points;
//}

osg::Geode * MeasureProfileAnalysis::lined(osg::Vec3 a, osg::Vec3 b)
{
	//创建保存几何信息的对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

	//创建四个顶点
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
	v->clear();
	v->push_back(a);
	v->push_back(b);//osg::Vec3(b[0], b[2], -b[1])

	geom->setVertexArray(v.get());

	//为每个顶点指定一种颜色
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
	c->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)); //坐标原点为红色
	c->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)); //x red

													 //如果没指定颜色则会变为黑色
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//line
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2)); //X

																				//point
	osg::ref_ptr<osg::Geometry> pointgeom = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> pv = new osg::Vec3Array();
	pv->push_back(a);
	pv->push_back(b);
	pointgeom->setVertexArray(pv.get());
	osg::ref_ptr<osg::Vec4Array> pc = new osg::Vec4Array();
	pc->push_back(osg::Vec4(0.98f, 0.5f, 0.04f, 1.0f));
	pc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	pointgeom->setColorArray(pc.get());
	pointgeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	pointgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 2));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.0), osg::StateAttribute::ON);


	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func      
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	geode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);


	osg::Point* pointSize = new osg::Point();
	pointSize->setSize(5.0);
	geode->getOrCreateStateSet()->setAttribute(pointSize, osg::StateAttribute::ON);



	geode->addDrawable(pointgeom.get());
	//geode->addDrawable(geom.get());

	geode->setName("lined");

	return geode.release();

}

osg::ref_ptr<osg::Vec3Array> MeasureProfileAnalysis::getInsPoints(MapNode * mapNode, osg::Vec3 start, osg::Vec3 end, int num)
{
	osg::ref_ptr<osg::Vec3Array> outPosArr1 = new osg::Vec3Array();
	outPosArr1->clear();
	osgEarth::GeoPoint insp2;
	//insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), insp);

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

			outPosArr1->push_back(insp);

			
			qDebug() << insp.x() << ":" << insp.y() << ":" << insp.z();
		}
	}

	return  outPosArr1;
}

osg::Geode * MeasureProfileAnalysis::drawEdges(osg::ref_ptr<osg::Vec3Array>  points)
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

osg::Geode * MeasureProfileAnalysis::createProFileGeode(osg::Vec3 start, osg::Vec3 end, const osg::Vec3Array *insPosArr)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	//shader
	osg::ref_ptr<osg::Program> profile_program = new osg::Program;
	profile_program->addShader(new osg::Shader(osg::Shader::VERTEX, vertShader));
	profile_program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));

	osg::Vec4 cord = osg::Vec4(1.0f, .0f, 1.0f, 1.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(profile_program, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	geode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::StateSet* stateset = geode->getOrCreateStateSet();

	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	//---------------------------------------
	osg::ref_ptr<osg::Geometry>  geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vArr = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> pc = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> nArr = new osg::Vec3Array;

	int num = insPosArr->size();
	osg::Vec3 dir(end[0] - start[0], end[1] - start[1], end[2] - start[2]);

	for (int i = 0; i < num; i++)
	{
		osg::Vec3 insp1 = insPosArr->asVector()[i];
		osg::Vec3 p_start = start + dir * float(i) / (num - 1);
		vArr->push_back(p_start);
		vArr->push_back(insp1);

		pc->push_back(osg::Vec4(1, 1, 0, 1));
		pc->push_back(osg::Vec4(1, 1, 0, 1));
	}

	geom->setVertexArray(vArr);
	geom->setColorArray(pc);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//geom->setNormalArray(nArr);
	//geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX); // BIND_OVERALL  BIND_PER_VERTEX

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, vArr->size()));  // TRIANGLE_STRIP TRIANGLE_FAN
	geode->addDrawable(geom.get());
	
	return geode.release();;
}

bool MeasureProfileAnalysis::buildModel()
{
	//osg::Geometry* lineString = (mGeometry);
	//if (NULL == lineString)
	//	return false;

	if (m_vecPoint.size() < 1)
		return false;

	//osgEarth::MapNode* mapNode = mapNode->getMap()->get;// (osgEarth::MapNode*)mERenderTF->getMapNode();
	//const SpatialReference* geoSRS = 
	
	mRoot->removeChildren(0, mRoot->getNumChildren());
	osg::Vec3d start = m_vecPoint[0];
	//lineString->getPoint(0, start.x(), start.y(), start.z());
	osgEarth::GeoPoint startpos(mapNode->getMapSRS(), start.x(), start.y(), 0, ALTMODE_ABSOLUTE);
	osg::Vec3d startWpos;
	startpos.toWorld(startWpos);
	

	osg::Vec3d end = m_vecPoint[1];
	//lineString->getPoint(1, end.x(), end.y(), end.z());
	osgEarth::GeoPoint endpos(mapNode->getMapSRS(), end.x(), end.y(), 0, ALTMODE_ABSOLUTE);
	osg::Vec3d endWpos;
	endpos.toWorld(endWpos);

	osg::ref_ptr<osg::Vec3Array> outPosArr = getInsPoints(mapNode, startWpos, endWpos, 100);

	osg::ref_ptr<osg::Vec3Array> posArr_ = new  osg::Vec3Array();
	posArr_->resize(outPosArr->size());
	for (size_t i = 0; i < outPosArr->size(); i++)
		posArr_->at(i) = outPosArr->asVector()[i] /** (1 + 0.001)*/;

	//  osg::Group * group = new osg::Group;
	osg::Geode *profilePlane = createProFileGeode(startWpos, endWpos, posArr_);
	mRoot->addChild(profilePlane);

	osg::Geode *edges = drawEdges(posArr_);
	mRoot->addChild(edges);

	//// 计算剖面最高点和最低点
	double lat, lon, hei;
	osg::ref_ptr<osg::EllipsoidModel> elpModel = new osg::EllipsoidModel;

	double maxH = 0.0;
	double minH = 1e9;
	int maxh = 0; int minh = 1e9;
	for (size_t h = 0; h < outPosArr->size(); h++)
	{
		elpModel->convertXYZToLatLongHeight(outPosArr->asVector()[h][0], outPosArr->asVector()[h][1], outPosArr->asVector()[h][2], lat, lon, hei);

		if (hei > maxH) { maxH = hei; maxh = h; }
		if (hei < minH) { minH = hei; minh = h; }

	}
	//是否绘制高低点
	osg::Geode *line = lined(posArr_->asVector()[maxh], posArr_->asVector()[minh]);
	mRoot->addChild(line);


	std::string  miny = std::to_string(minH);
	std::string mintxt = " min: ";
	mintxt = mintxt + miny + "\n";

	std::string  maxy = std::to_string(maxH);
	std::string maxtxt = " max: ";
	maxtxt = maxtxt + maxy + "\n";

	osg::ref_ptr<osgText::Text> minText = new osgText::Text;
	minText->setText(mintxt, osgText::String::ENCODING_UTF8);
	minText->setFont("fonts/Arial.ttf");
	minText->setCharacterSize(100.0);
	minText->setColor(osg::Vec4(1.0, 0.0, 1.0, 0.8));

	osg::ref_ptr<osgText::Text> maxText = new osgText::Text;
	maxText->setText(maxtxt, osgText::String::ENCODING_UTF8);
	maxText->setFont("fonts/Arial.ttf");
	maxText->setCharacterSize(100.0);
	maxText->setColor(osg::Vec4(1.0, 0.0, 1, 0.8));
	maxText->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION);

	osg::Vec3d xx = osg::Vec3d(1, 0, 0);
	osg::Vec3d zz = osg::Vec3d(0, 0, 1);
	xx.normalize();
	zz.normalize();

	osg::Vec3d linedir = endWpos - startWpos;
	linedir.normalize();

	//------
	osg::Vec3d axis = startWpos;
	axis.normalize();
	osg::Vec3d planeNor = axis^linedir;
	planeNor.normalize();
	//------

	osg::Quat quat;
	quat.makeRotate(osg::Vec3d(1, 0, 0), linedir);

	osg::Vec3d rotzz = zz*osg::Matrix::rotate(xx, linedir);
	osg::Quat quat2;
	quat2.makeRotate(rotzz, -planeNor);

	minText->setRotation(quat*quat2);
	minText->setPosition(posArr_->asVector()[minh] * (1 - 0.0001));


	maxText->setRotation(quat*quat2);
	maxText->setPosition(posArr_->asVector()[maxh]);

	profilePlane->addDrawable(minText.get());
	profilePlane->addDrawable(maxText.get());

	/*mModelNode = new CRyNode(mRoot);
	mModelNode->setName(RY_TEXT("GrElementModelNode:") + mName);
	mModelNode->setUserData(RY_TEXT("EMeasureGrElement"), CRyVariant((size_t)this));
	mModelGroupNode->setName(RY_TEXT("Measure"));
	mModelGroupNode->addChildRef(SAFE_ADDREF(mModelNode.get()));*/
	return true;
}

void MeasureProfileAnalysis::updateFeatureNode()
{
	//CRyLineString* lineString = dynamic_cast<CRyLineString*>(mGeometry);
	//if (NULL == lineString)
	//	return;

	//osgEarth::MapNode* mapNode = (osgEarth::MapNode*)mERenderTF->getMapNode();
	const SpatialReference* geoSRS = mapNode->getMapSRS();
	mRoot->removeChildren(0, mRoot->getNumChildren());


	osg::Vec3d start = m_vecPoint[0];
	//lineString->getPoint(0, start.x(), start.y(), start.z());
	osgEarth::GeoPoint startpos(geoSRS, start.x(), start.y(), 0, ALTMODE_ABSOLUTE);
	osg::Vec3d startWpos;
	startpos.toWorld(startWpos);

	osg::Vec3d end = m_vecPoint[1];
	//lineString->getPoint(1, end.x(), end.y(), end.z());
	osgEarth::GeoPoint endpos(geoSRS, end.x(), end.y(), 0, ALTMODE_ABSOLUTE);
	osg::Vec3d endWpos;
	endpos.toWorld(endWpos);

	//outPosArr = new  osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> outPosArr = getInsPoints(mapNode, startWpos, endWpos, 50);

	osg::Vec3Array *posArr_ = new  osg::Vec3Array();
	posArr_->resize(outPosArr->size());
	for (size_t i = 0; i < outPosArr->size(); i++)
		posArr_->at(i) = outPosArr->asVector()[i] /** (1 + 0.001)*/;

	osg::Geode *profilePlane = createProFileGeode(startWpos, endWpos, posArr_);
	mRoot->addChild(profilePlane);
	if (posArr_->size() < 1)
		return;
	osg::Geode *edges = drawEdges(posArr_);
	mRoot->addChild(edges);

	//// 计算剖面最高点和最低点
	double lat, lon, hei;
	osg::ref_ptr<osg::EllipsoidModel> elpModel = new osg::EllipsoidModel;

	double maxH = 0.0;
	double minH = 1e9;
	int maxh = 0; int minh = 1e9;
	for (size_t h = 0; h < outPosArr->size(); h++)
	{
		elpModel->convertXYZToLatLongHeight(outPosArr->asVector()[h][0], outPosArr->asVector()[h][1], outPosArr->asVector()[h][2], lat, lon, hei);

		if (hei > maxH) { maxH = hei; maxh = h; }
		if (hei < minH) { minH = hei; minh = h; }

	}
	//是否绘制高低点
	osg::Geode *line = lined(posArr_->asVector()[maxh], posArr_->asVector()[minh]);
	mRoot->addChild(line);

	std::string  miny = std::to_string(minH);
	std::string mintxt = " min: ";
	mintxt = mintxt + miny + "\n";

	std::string  maxy = std::to_string(maxH);
	std::string maxtxt = " max: ";
	maxtxt = maxtxt + maxy + "\n";

	osg::ref_ptr<osgText::Text> minText = new osgText::Text;
	minText->setText(mintxt, osgText::String::ENCODING_UTF8);
	minText->setFont("fonts/Arial.ttf");
	minText->setCharacterSize(200.0);
	minText->setColor(osg::Vec4(1.0, 0.0, 1.0, 0.8));

	osg::ref_ptr<osgText::Text> maxText = new osgText::Text;
	maxText->setText(maxtxt, osgText::String::ENCODING_UTF8);
	maxText->setFont("fonts/Arial.ttf");
	maxText->setCharacterSize(200.0);
	maxText->setColor(osg::Vec4(1.0, 0.0, 1, 0.8));
	maxText->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION);

	osg::Vec3d xx = osg::Vec3d(1, 0, 0);
	osg::Vec3d zz = osg::Vec3d(0, 0, 1);
	xx.normalize();
	zz.normalize();

	osg::Vec3d linedir = endWpos - startWpos;
	linedir.normalize();

	//------
	osg::Vec3d axis = startWpos;
	axis.normalize();
	osg::Vec3d planeNor = axis^linedir;
	planeNor.normalize();
	//------

	osg::Quat quat;
	quat.makeRotate(osg::Vec3d(1, 0, 0), linedir);

	osg::Vec3d rotzz = zz*osg::Matrix::rotate(xx, linedir);
	osg::Quat quat2;
	quat2.makeRotate(rotzz, -planeNor);

	minText->setRotation(quat*quat2);
	minText->setPosition(posArr_->asVector()[minh] * (1 - 0.0001));

	maxText->setRotation(quat*quat2);
	maxText->setPosition(posArr_->asVector()[maxh]);

	profilePlane->addDrawable(minText.get());
	profilePlane->addDrawable(maxText.get());

	//mModelNode = new CRyNode(group);
}
