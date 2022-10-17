#include "MeasureGradientAnalysis.h"
#include <osgEarth/MapNode>
#include <osgEarth/LabelNode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/Depth>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Point>
#include "qdebug.h"
#include <osgEarth/GeoMath>
#include <osg/CullFace>
#include <osgEarth/ImageOverlay>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgEarth/Registry>
#include <math.h>
#include "transcoder.h"

MeasureGradientAnalysis::MeasureGradientAnalysis(osg::Group * group, MapNode * node)
{
	this->mapNode = node;
	this->mRoot = group;
	m_pRectangleNode = nullptr;
//	_ptr = nullptr;

	m_nAddHeight = 20;	//分析
	m_nCut = 10;
	//矩形样式
	mRectangleStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = Color(osgEarth::Color::Yellow, 1);
	AltitudeSymbol* alt = mRectangleStyle.getOrCreate<AltitudeSymbol>();
	alt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
	alt->verticalOffset() = 0.1;
	alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	RenderSymbol* render = mRectangleStyle.getOrCreate<RenderSymbol>();
	render->minAlpha() = 0.15f;
	//LineSymbol* ls = mRectangleStyle.getOrCreate<LineSymbol>();

	//ls->stroke()->color() = Color(osgEarth::Color::Blue);

	//ls->stroke()->width() = 1.0f;
	//ls->stroke()->widthUnits() = Units::PIXELS;
	//ls->tessellation() = 20;

	//颜色渲染
	m_tf = new osg::TransferFunction1D();
	m_tf->setColor(900, osgEarth::Color::Green);
	m_tf->setColor(950, osgEarth::Color::Yellow);
	m_tf->setColor(980, osgEarth::Color::Red);



}

MeasureGradientAnalysis::~MeasureGradientAnalysis()
{
	
}

osg::Drawable* MeasureGradientAnalysis::loadArrow()
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("02.png");
	texture->setImage(image.get());

	osg::ref_ptr<osg::Drawable> quad = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

	return quad.release();
	
}

void MeasureGradientAnalysis::onLeftButton(GeoPoint * Point)
{
	if (pointNum == 0)
	{
		mStartPos = osg::Vec3d(Point->x(), Point->y(), Point->alt());

		if (m_pRectangleNode == nullptr)
		{
			CreateRectAngle(*Point);
		}
		

	}
	else
	{
		GeoPoint start = GeoPoint(mapNode->getMapSRS(), mStartPos);
		osgEarth::RectangleNode::Corner index = getPointCorner1(*Point);
		m_pRectangleNode->setCorner(index, *Point);
		if (index == 0)
		{
			m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_UPPER_RIGHT, start);
		}
		else if (index == 1)
		{
			m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_UPPER_LEFT, start);
		}
		else if (index == 2)
		{
			m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_LOWER_RIGHT, start);
		}
		else if (index == 3)
		{
			m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_LOWER_LEFT, start);
		}

		GeoPoint p1 = m_pRectangleNode->getCorner(RectangleNode::CORNER_UPPER_LEFT);
		GeoPoint p2 = m_pRectangleNode->getCorner(RectangleNode::CORNER_LOWER_LEFT);
		GeoPoint p3 = m_pRectangleNode->getCorner(RectangleNode::CORNER_LOWER_RIGHT);
		GeoPoint p4 = m_pRectangleNode->getCorner(RectangleNode::CORNER_UPPER_RIGHT);
		CurRectAngle(p1,p2,p3,p4);

	}
	pointNum++;
}

void MeasureGradientAnalysis::onDoubleClick(GeoPoint * Point)
{
	onLeftButton(Point);
	this->m_bFinish = true;
	//if (m_pRectangleNode == nullptr)
	//	return;
	//mRoot->removeChild(m_pRectangleNode);
}

void MeasureGradientAnalysis::onMouseMove(GeoPoint * Point)
{
	if (pointNum < 1)
		return;
	if (m_pRectangleNode == nullptr)
		return;

	GeoPoint start = GeoPoint(mapNode->getMapSRS(), mStartPos);
	osgEarth::RectangleNode::Corner index = getPointCorner1(*Point);
	m_pRectangleNode->setCorner(index, *Point);
	if (index == 0)
	{
		m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_UPPER_RIGHT, start);
	}
	else if (index == 1)
	{
		m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_UPPER_LEFT, start);
	}
	else if (index == 2)
	{
		m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_LOWER_RIGHT, start);
	}
	else if (index == 3)
	{
		m_pRectangleNode->setCorner(osgEarth::RectangleNode::CORNER_LOWER_LEFT, start);
	}
}

void MeasureGradientAnalysis::CreateRectAngle(GeoPoint end)
{
	GeoPoint start = GeoPoint(mapNode->getMapSRS(), mStartPos);

	m_pRectangleNode = new RectangleNode(start,
		Distance(0, Units::KILOMETERS),
		Distance(0, Units::KILOMETERS),
		mRectangleStyle);
	m_pRectangleNode->setCorner(getPointCorner1(end), end);


	mRoot->addChild(m_pRectangleNode);
	return;
}

void MeasureGradientAnalysis::CurRectAngle(GeoPoint p1, GeoPoint p2, GeoPoint p3, GeoPoint p4)
{
	double Height = 0.0;
	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p1.x(), p1.y(), &Height);
	p1.z() = Height + m_nAddHeight;
	GeoPoint point1 = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), osg::Vec3d(p1.x(),p1.y(),p1.z()));

	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p2.x(), p2.y(), &Height);
	p2.z() = Height + m_nAddHeight;
	GeoPoint point2 = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), osg::Vec3d(p2.x(), p2.y(), p2.z()));

	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p3.x(), p3.y(), &Height);
	p3.z() = Height + m_nAddHeight;
	GeoPoint point3 = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), osg::Vec3d(p3.x(), p3.y(), p3.z()));

	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p4.x(), p4.y(), &Height);
	p4.z() = Height + m_nAddHeight;
	GeoPoint point4 = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), osg::Vec3d(p4.x(), p4.y(), p4.z()));

	int nCut = m_nCut;
	
	
	//↓
	osg::Vec3d startWpos;
	point1.toWorld(startWpos);
	osg::Vec3d endWpos;
	point2.toWorld(endWpos);
	osg::ref_ptr<osg::Vec3Array> outPosArr1 = getInsPoints(mapNode, startWpos, endWpos, m_nCut);
	//↓
	point4.toWorld(startWpos);
	point3.toWorld(endWpos);
	osg::ref_ptr<osg::Vec3Array> outPosArr2 = getInsPoints(mapNode, startWpos, endWpos, m_nCut);
	osg::ref_ptr<osg::Vec3Array> outPosArr = new osg::Vec3Array();;
	int count = outPosArr1->size();
	GeoPoint insp2;
	for (int i = 0; i < count; i++)
	{
		osg::Vec3d pt1 = outPosArr1->at(i);
		osg::Vec3d pt2 = outPosArr2->at(i);

		getInsPoints(mapNode, pt1, pt2, m_nCut, outPosArr);
		qDebug() << outPosArr->size();
	}
	qDebug() << outPosArr->size(); 
	for (int i = 0; i < outPosArr->size(); ++i)
	{
		qDebug()<<outPosArr->at(i).x()<< outPosArr->at(i).y()<< outPosArr->at(i).z();
	}
	//loadContourMap(outPosArr);
	loadContourImage(outPosArr);
	pointNum = -1;
	mRoot->removeChild(m_pRectangleNode);
	m_pRectangleNode = nullptr;
}

osg::ref_ptr<osg::Vec3Array> MeasureGradientAnalysis::getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num)
{
	GeoPoint insp2;
	osg::ref_ptr<osg::Vec3Array> outPosArr1 = new osg::Vec3Array();
	outPosArr1->clear();
	double Height = 0.0;
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

			Height = 0.0;
			insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), insp);
			mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), insp2.x(), insp2.y(), &Height);
			insp2.z() = Height + m_nAddHeight;
			insp2.toWorld(insp);

			outPosArr1->push_back(insp);
		}
	}
	outPosArr1->push_back(end);
	return  outPosArr1;
}
void  MeasureGradientAnalysis::getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num, osg::ref_ptr<osg::Vec3Array> array)
{
	double Height = 0.0;
	GeoPoint insp2;
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

			Height = 0.0;
			insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), insp);
			mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), insp2.x(), insp2.y(), &Height);
			insp2.z() = Height + m_nAddHeight;
			insp2.toWorld(insp);
			array->push_back(insp);
			qDebug() << insp2.x() << insp2.y() << insp2.z();
			qDebug() << insp.x()<< insp.y()<< insp.z();
		}
	}
	array->push_back(end);
	for (int i = 0 ; i < array->size();++i)
	{
		qDebug() << array->at(i).x() << array->at(i).y() << array->at(i).z();
	}
}

void MeasureGradientAnalysis::loadContourMap(osg::ref_ptr<osg::Vec3Array> array)
{
	//载入点坐标
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

	//设置顶点数据
	geom->setVertexArray(array.get());
	for (int i = 0; i < array->size();++i)
	{
		qDebug() << array.get()->at(i).x() << array.get()->at(i).y() << array.get()->at(i).z();
	}
	
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();
	//添加数据
	//颜色映射
	GeoPoint insp;
	double dmax = 0.0, dmin =1000000;
	for (int i = 0; i<array->size(); i++)
	{
		float x, y;
		osg::Vec3d pp0 = array->at(i);
		insp.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp0);
		if (dmax < insp.z())
		{
			dmax = insp.z();
		}
		if (dmin > insp.z())
		{
			dmin = insp.z();
		}
		//qDebug() << insp.z();
		osg::Vec4 pp4 = m_tf->getColor(insp.z());
		qDebug() << pp4.x() << pp4.y() << pp4.z() << pp4.w();
		vc->push_back(pp4);
		/*vc->push_back(m_tf->getColor(insp.z()));*/
	}
	qDebug() << dmax<<":"<<dmin;
	//设置颜色数组
	geom->setColorArray(vc.get());
	//设置颜色的绑定方式为单个顶点
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//设置法线的绑定方式为全部顶点
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	// 添加图元，绘图基元为四边形
	int nCut = m_nCut+1;
	for (int i = 0; i <nCut; i++)
	{
		for (int j = 0; j <nCut; j++)
		{
			osg::DrawElementsUInt* Face4 = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
			Face4->push_back(i*nCut + j);
			Face4->push_back((i + 1)*nCut + j);
			Face4->push_back((i + 1)*nCut + j +1);
			Face4->push_back(i*nCut + j + 1);
			geom->addPrimitiveSet(Face4);
		}
	}


	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	//开启混合
	//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::ref_ptr<osg::CullFace> cullface=new osg::CullFace(osg::CullFace::BACK);
	stateset->setAttribute(cullface.get());
	stateset->setMode(GL_CULL_FACE, 0);
//	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, array->size()));
	//添加到叶节点
	geode->addDrawable(geom.get());

	mRoot->addChild(geode.get());
}

void MeasureGradientAnalysis::loadContourImage(osg::ref_ptr<osg::Vec3Array> array)
{
	//长宽大小
	int side1 = sqrt(array->size());
	osg::ref_ptr<osg::Image> image1 = new osg::Image();
//	if (_ptr)
	//	delete _ptr;
	unsigned char* ptr = new unsigned char[array->size() *3];
	osg::Drawable* arrow=loadArrow();
	
	for (int i = 0; i<array->size(); i++)
	{
		GeoPoint insp;
		osg::Vec3d pp0 = array->at(i);
		insp.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp0);
		
		osg::Vec4 pp4 = m_tf->getColor(insp.z());
		ptr[i*3] = (unsigned char)(pp4.r()*255);
		ptr[i * 3+1] = (unsigned char)(pp4.g()*255);
		ptr[i * 3 + 2] = (unsigned char)(pp4.b()*255);
	//	*_ptr++ = (unsigned int)(pp4.a() * 255);
	
	}
	

	image1->setImage(side1, side1, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, ptr, osg::Image::USE_NEW_DELETE);
	
	osgEarth::ImageOverlay* overlay1 = new osgEarth::ImageOverlay(mapNode, image1);
	overlay1->setAlpha(0.5);
	overlay1->setBounds(osgEarth::Bounds(m_pRectangleNode->getLowerLeft().x(), m_pRectangleNode->getLowerLeft().y(), m_pRectangleNode->getUpperRight().x(), m_pRectangleNode->getUpperRight().y()));


	mRoot->addChild(overlay1);

	osg::Group* arrows = new osg::Group();
	arrows->setName("gradient");
	for (int i = 1; i<side1 - 1; i++)
		for (int j = 1; j < side1 - 1; j++)
		{
			GeoPoint insp;
			osg::Vec3d pp0 = array->at(i*side1 + j);
			insp.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp0);

			insp.alt() += 3;

			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			geode->addDrawable(arrow);

			osg::MatrixTransform* pat = new osg::MatrixTransform();
			

			//pat->addChild(gg);
			double angle = getAngle(array, i, j);
			double podu = getPodu(array, i, j);
			double poxiang = getPoxiang(array, i, j);

			std::string ss = "坡度：" + std::to_string(podu) + "\n" + "坡向：" + std::to_string(poxiang);
			std::string ss1;
			Transcoder::gb2312ToUtf8(ss, ss1);
			pat->setName(ss1);
			pat->setMatrix(osg::Matrix::scale(20, 20, 20)
				*osg::Matrix::rotate(osg::inDegrees(angle), osg::Z_AXIS));
			//	*Matrix::rotate(osg::inDegrees(rph.y()), osg::X_AXIS)
			//	*Matrix::rotate(osg::inDegrees(rph.z()), osg::Y_AXIS));
			osg::AutoTransform* at = new osg::AutoTransform;
			at->setAutoScaleToScreen(true);
			//at->setMaximumScale(1000);
			//at->setMaximumScale(100);

			geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				osg::ref_ptr<osgEarth::GeoTransform> xform = new osgEarth::GeoTransform();

				xform->setTerrain(mapNode->getTerrain());

				//xform->addChild(model);
				//osgEarth::GeoPoint point(sceneMan_3d->getMapNode()->getMapSRS()->getGeographicSRS(), mVecPos.x(), mVecPos.y(), mVecPos.z());                 //使用绝对高，正高
				xform->setPosition(insp);
			
				osgEarth::Registry::shaderGenerator().run(geode);
				pat->addChild(geode);
				at->addChild(pat);
				xform->addChild(at);
				arrows->addChild(xform);
		}
	mRoot->addChild(arrows);
	//delete(ptr);
	//delete(image1);
	//_array = array;
}

//获取长宽距离
void MeasureGradientAnalysis::getWidthHeight(osg::ref_ptr<osg::Vec3Array> array, double& width, double& height)
{
	GeoPoint insp0;
	osg::Vec3d pp0 = array->at(0);
	insp0.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp0);

	GeoPoint insp1;
	osg::Vec3d pp1 = array->at(array->size() - 1);
	insp1.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp1);

	GeoPoint insp2(mapNode->getMapSRS()->getGeographicSRS());
	insp2.x() = insp0.x();
	insp2.y() = insp1.y();
	insp2.alt() = (insp0.alt() + insp1.alt()) / 2;

	double d1 = GeoMath::distance(osg::DegreesToRadians(insp2.y()), osg::DegreesToRadians(insp2.x()),
		osg::DegreesToRadians(insp0.y()), osg::DegreesToRadians(insp0.x()));
	double d2 = GeoMath::distance(osg::DegreesToRadians(insp2.y()), osg::DegreesToRadians(insp2.x()),
		osg::DegreesToRadians(insp1.y()), osg::DegreesToRadians(insp1.x()));

	width = d2;
	height = d1;
}
//获取旋转角度
double MeasureGradientAnalysis::getAngle(osg::ref_ptr<osg::Vec3Array> array, int i, int j)
{
	//计算旋转角
	
	double d1, d2;
	getWidthHeight(array, d2, d1);
	double beta= atan(d1 / d2);

	int side1 = sqrt(array->size());
	double max = -1000;
	int maxX=0;
	int maxY=0;
	for(int m=-1;m<=1;m++)
		for (int n = -1; n <= 1; n++)
		{
			GeoPoint insp3;
			osg::Vec3d pp3 = array->at((i+m)*side1+j+n);
			insp3.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp3);
			if (insp3.alt() > max)
			{ 
				max = insp3.alt();
				maxX = m;
				maxY = n;
			}
		}
	//double max = array->at((i - 1)*side1 + j - 1).;
	/*if (maxX == 0 && maxY == 0 || maxX == 0 && maxY == 1)
		return 0;*/
	if (maxX == 1 && maxY == 1)
		return osg::RadiansToDegrees(beta);
	if (maxX == 1 && maxY == 0)
		return 90.;
	if(maxX == 1 && maxY == -1)
		return 180- osg::RadiansToDegrees(beta);
	if (maxX == 0 && maxY == -1)
		return 180;
	if(maxX == -1 && maxY == -1)
		return 180+ osg::RadiansToDegrees(beta);
	if (maxX == -1 && maxY == 0)
		return -90;
	if (maxX == -1 && maxY == 0)
		return -osg::RadiansToDegrees(beta);
	return 0;
}

//计算坡度
double MeasureGradientAnalysis::getPodu(osg::ref_ptr<osg::Vec3Array> array,int i, int j)
{
	double d1, d2;
	getWidthHeight(array, d2, d1);

	int side1 = sqrt(array->size());
	double d3 = (d1 + d2) / side1/2;

	GeoPoint insp4;
	osg::Vec3d pp4 = array->at((i -1)*side1 + j);
	insp4.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp4);
	double z4 = insp4.alt();

	GeoPoint insp1;
	osg::Vec3d pp1 = array->at(i*side1 + j-1);
	insp1.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp1);
	double z1 = insp1.alt();

	GeoPoint insp2;
	osg::Vec3d pp2 = array->at((i + 1)*side1 + j);
	insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp2);
	double z2 = insp2.alt();

	GeoPoint insp3;
	osg::Vec3d pp3 = array->at(i*side1 + j+1);
	insp3.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp3);
	double z3 = insp3.alt();

	return osg::RadiansToDegrees(atan(sqrt(pow(z1 - z3, 2) + pow(z4 - z2, 2)) / d3));
}
//计算坡向
double MeasureGradientAnalysis::getPoxiang(osg::ref_ptr<osg::Vec3Array> array,int i, int j)
{
	double d1, d2;
	getWidthHeight(array, d2, d1);

	int side1 = sqrt(array->size());
	double d3 = (d1 + d2) / side1;

	GeoPoint insp4;
	osg::Vec3d pp4 = array->at((i - 1)*side1 + j);
	insp4.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp4);
	double z4 = insp4.alt();

	GeoPoint insp1;
	osg::Vec3d pp1 = array->at(i*side1 + j - 1);
	insp1.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp1);
	double z1 = insp1.alt();

	GeoPoint insp2;
	osg::Vec3d pp2 = array->at((i + 1)*side1 + j);
	insp2.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp2);
	double z2 = insp2.alt();

	GeoPoint insp3;
	osg::Vec3d pp3 = array->at(i*side1 + j + 1);
	insp3.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), pp3);
	double z3 = insp3.alt();

	return osg::RadiansToDegrees(atan((z4 - z2) / (z1 - z3))) + 90;
}

void MeasureGradientAnalysis::renderCircleNode(osg::Vec3d  vec3d)
{
	GeoPoint insp;
	insp.fromWorld(mapNode->getMapSRS()->getGeographicSRS(), vec3d);
	qDebug() << insp.x() << ":" << insp.y() << ":" << insp.z();

	GeoPoint point = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), vec3d);
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
	_transfer->setMatrix(osg::Matrixd::translate(vec3d.x(), vec3d.y(), vec3d.z()));
	_transfer->addChild(at);

	mRoot->addChild(_transfer);
}

osgEarth::RectangleNode::Corner MeasureGradientAnalysis::getPointCorner1(const osgEarth::GeoPoint & corner)
{
	GeoPoint center = GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), mStartPos);

	//Figure out the new width and height
	double earthRadius = center.getSRS()->getEllipsoid()->getRadiusEquator();

	double lat = osg::DegreesToRadians(center.y());
	double lon = osg::DegreesToRadians(center.x());
	double halfWidthMeters = m_pRectangleNode->getWidth().as(Units::METERS) / 2.0;
	double halfHeightMeters = m_pRectangleNode->getHeight().as(Units::METERS) / 2.0;

	double eastLon, eastLat;
	double westLon, westLat;
	double northLon, northLat;
	double southLon, southLat;

	//Get the current corners
	GeoMath::destination(lat, lon, osg::DegreesToRadians(90.0), halfWidthMeters, eastLat, eastLon, earthRadius);
	GeoMath::destination(lat, lon, osg::DegreesToRadians(-90.0), halfWidthMeters, westLat, westLon, earthRadius);
	GeoMath::destination(lat, lon, osg::DegreesToRadians(0.0), halfHeightMeters, northLat, northLon, earthRadius);
	GeoMath::destination(lat, lon, osg::DegreesToRadians(180.0), halfHeightMeters, southLat, southLon, earthRadius);

	if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) > southLat) {
		return RectangleNode::CORNER_UPPER_LEFT;
	}
	else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) > southLat) {
		return RectangleNode::CORNER_UPPER_RIGHT;
	}
	else if (osg::DegreesToRadians(corner.x()) < eastLon && osg::DegreesToRadians(corner.y()) < northLat) {
		return RectangleNode::CORNER_LOWER_LEFT;
	}
	else if (osg::DegreesToRadians(corner.x()) > westLon && osg::DegreesToRadians(corner.y()) < northLat) {
		return RectangleNode::CORNER_LOWER_RIGHT;
	}
}
