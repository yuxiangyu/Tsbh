//////////////////////////////////////////////////////////////////////////
/// Copyright (C), 1998-2016, rytec Corporation. All rights reserved.
/// \file       RichLabelNode.cpp
/// \brief      
/// \author     LiXingQiang
/// \version    0.95
/// \date       2019-3
//////////////////////////////////////////////////////////////////////////

#include "RichLabelNode.h"
//#include "Base/Log.h"

#include <osgEarth/AnnotationUtils>
#include <osgEarth/AnnotationRegistry>
#include <osgEarth/BboxDrawable>
#include <osgEarth/Color>
#include <osgEarth/BBoxSymbol>
#include <osgEarth/Registry>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/GeoMath>
#include <osgEarth/Utils>
#include <osgEarth/ScreenSpaceLayout>
#include <osgText/Text>
#include <osg/Depth>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/LineWidth>

#include <QString>


using namespace osgEarth;


RichLabelNode::RichLabelNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
	const Style& style)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
{
	init(style);
	setPosition(position);
	mapNode->addChild(this);
}

RichLabelNode::RichLabelNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
	const TextSymbol* symbol)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
{
	Style style;
	style.add(const_cast<TextSymbol*>(symbol));

	init(style);
	setPosition(position);
	mapNode->addChild(this);
}

RichLabelNode::RichLabelNode(MapNode* mapNode, const Config& conf, const osgDB::Options* dbOptions)
	: mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
{
	optional<Style> style;

	conf.get("style", style);
	conf.get("text", mText);

	init(*style);
	mapNode->addChild(this);
}

RichLabelNode::RichLabelNode(osg::Group* Group, const GeoPoint& position, const std::string& text,
	const Style& style)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
{
	init(style);
	setPosition(position);
	Group->addChild(this);
}

RichLabelNode::RichLabelNode(const RichLabelNode& rhs, const osg::CopyOp& op)
	: GeoPositionNode(rhs, op)
	, mLabelRotationRad(0.)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
{
	//nop - unused
}


RichLabelNode* RichLabelNode::buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode)
{
	Style labelStyle;
	labelStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";  // 暂时
	labelStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_LEFT_CENTER;
	labelStyle.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1.0, 1.0, 0.1);
	labelStyle.getOrCreate<BBoxSymbol>()->border()->color() = Color(1.0, 1.0, 1.0, 0.8);
	labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(2, 100);

	auto label = new RichLabelNode(mapNode, pos, text, labelStyle);
	label->setDynamic(true);
	root->addChild(label);
	return label;
}

void RichLabelNode::init(const Style& style)
{
	ScreenSpaceLayout::activate(this->getOrCreateStateSet());
	mGeode = new osg::Geode();

	// ensure that (0,0,0) is the bounding sphere control/center point.
	// useful for things like horizon culling.
	mGeode->setComputeBoundingSphereCallback(new ControlPointCallback());

	getPositionAttitudeTransform()->addChild(mGeode.get());

	osg::StateSet* stateSet = mGeode->getOrCreateStateSet();
	stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);

	setStyle(style);
}

void RichLabelNode::setText(const std::string& text)
{
	if (!_dynamic && getNumParents() > 0)
	{
		//    LOG_ERROR(RY_TEXT("无法修改非动态标注节点"));
		return;
	}

	if (mTextGeometry)
	{
		const TextSymbol* symbol = mStyle.get<TextSymbol>();
		osgText::String::Encoding textEncoding = osgText::String::ENCODING_UTF8;
		if (NULL != symbol && symbol->encoding().isSet())
		{
			textEncoding = AnnotationUtils::convertTextSymbolEncoding(symbol->encoding().value());
		}

		mTextGeometry->setText(text, textEncoding);

		mTextGeometry->dirtyGLObjects();
		mText = text;

		const BBoxSymbol* bboxsymbol = mStyle.get<BBoxSymbol>();

		auto& bbox = mTextGeometry->getBoundingBox();
		if (bboxsymbol && this->mTextGeometry)
		{
			if (mBBoxGeom.get())
			{
				mGeode->removeChild(mBBoxGeom);
				mGeode->removeChild(mTextGeometry);
			}
			mBBoxGeom = new BboxDrawable(bbox, *bboxsymbol);
			mGeode->addDrawable(mBBoxGeom);
			mGeode->addDrawable(mTextGeometry);
		}
		applyStyle(mStyle);
		updateLayoutData();
	}
}

void RichLabelNode::setStyle(const Style& style)
{
	if (!_dynamic && getNumParents() > 0)
	{
		//    LOG_ERROR(RY_TEXT("无法修改非动态标注节点"));
		return;
	}

	mGeode->removeDrawables(0, mGeode->getNumDrawables());

	mStyle = style;

	const TextSymbol* symbol = mStyle.get<TextSymbol>();

	if (mText.empty())
		mText = symbol->content()->eval();

	if (symbol && symbol->onScreenRotation().isSet())
	{
		mLabelRotationRad = osg::DegreesToRadians(symbol->onScreenRotation()->eval());
	}
	// In case of a label must follow a course on map, we project a point from the position
	// with the given bearing. Then during culling phase we compute both points on the screen
	// and then we can deduce the screen rotation
	// may be optimized...
	else if (symbol && symbol->geographicCourse().isSet())
	{
		mFollowFixedCourse = true;
		mLabelRotationRad = osg::DegreesToRadians(symbol->geographicCourse()->eval());
	}

	// 设置文本内容的时候没有更新该drawable
	QString text = QString::fromLocal8Bit(mText.c_str());

	mTextGeometry = (osgText::Text*)AnnotationUtils::createTextDrawable(text.toStdString(), symbol, osg::Vec3(0, 0, 0));
	mGeode->addDrawable(mTextGeometry);

	const BBoxSymbol* bboxSymbol = mStyle.get<BBoxSymbol>();
	auto& boundingBox = mTextGeometry->getBoundingBox();
	if (bboxSymbol && mTextGeometry)
	{
		mBBoxGeom = new BboxDrawable(boundingBox, *bboxSymbol);
		mGeode->addDrawable(mBBoxGeom);
	}
	mGeode->setCullingActive(false);

	applyStyle(mStyle);
	//setLightingIfNotSet(false);
	Registry::shaderGenerator().run(this, "osgEarth.RichLabelNode", Registry::stateSetCache());
	updateLayoutData();

	if (bboxSymbol && mTextGeometry)
	{
		auto margin = bboxSymbol->margin().get() - 1;
		osg::ref_ptr<osg::Geometry>(geometry_line) = new osg::Geometry();
		geometry_line->setUseVertexBufferObjects(true);

		osg::Vec3 anchor(-mDataLayout->getPixelOffset().r(), -mDataLayout->getPixelOffset().g(), 0.0);
		osg::Vec3 left_top(boundingBox.xMin() - margin, boundingBox.yMax() + margin, boundingBox.zMax());
		osg::Vec3 left_bottom(boundingBox.xMin() - margin, boundingBox.yMin() - margin, boundingBox.zMax());
		osg::Vec3 right_top(boundingBox.xMax() + margin, boundingBox.yMax() + margin, boundingBox.zMax());
		osg::Vec3 right_bottom(boundingBox.xMax() + margin, boundingBox.yMin() - margin, boundingBox.zMax());
		auto a_to_left_top = (left_top - anchor).length2();
		auto a_to_left_bottom = (left_bottom - anchor).length2();
		auto a_to_right_top = (right_top - anchor).length2();
		auto a_to_right_bottom = (right_bottom - anchor).length2();
		osg::Vec3 proj = left_top;
		auto current_distance = a_to_left_top;
		if (a_to_left_bottom < current_distance)
		{
			proj = left_bottom;
			current_distance = a_to_left_bottom;
		}
		if (a_to_right_top < current_distance)
		{
			proj = right_top;
			current_distance = a_to_right_top;
		}
		if (a_to_right_bottom < current_distance)
		{
			proj = right_bottom;
			current_distance = a_to_right_bottom;
		}

		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		v->push_back(osg::Vec3(-mDataLayout->getPixelOffset().r(), -mDataLayout->getPixelOffset().g(), 0.0));
		v->push_back(proj);
		geometry_line->setVertexArray(v);
		if (v->getVertexBufferObject())
			v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);

		auto c = new osg::Vec4Array();
		if (bboxSymbol->border().isSet())
		{
			c->push_back(bboxSymbol->border()->color());
			if (bboxSymbol->border()->width().isSet())
				getOrCreateStateSet()->setAttribute(new osg::LineWidth(bboxSymbol->border()->width().value()));
			geometry_line->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, v->getNumElements()));
		}
		geometry_line->setColorArray(c);
		geometry_line->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

		// add the static "isText=true" uniform; this is a hint for the annotation shaders
		// if they get installed.
		static osg::ref_ptr<osg::Uniform> s_isTextUniform = new osg::Uniform();
		s_isTextUniform->set(false);
		getOrCreateStateSet()->addUniform(s_isTextUniform.get());

		// Disable culling since this bounding box will eventually be drawn in screen space.
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
		setCullingActive(false);
#endif
		mGeode->addDrawable(geometry_line);
	}
	dirty();
}

void RichLabelNode::dirty()
{
	GeoPositionNode::dirty();
	updateLayoutData();
}

void RichLabelNode::setPriority(float value)
{
	GeoPositionNode::setPriority(value);
	updateLayoutData();
}

void RichLabelNode::updateLayoutData()
{
	if (!mDataLayout.valid())
		mDataLayout = new ScreenSpaceLayoutData();

	// re-apply annotation drawable-level stuff as neccesary.
	for (unsigned i = 0; i < mGeode->getNumDrawables(); ++i)
		mGeode->getDrawable(i)->setUserData(mDataLayout.get());

	mDataLayout->setPriority(getPriority());

	GeoPoint location = getPosition();
	location.makeGeographic();
	double latRad;
	double longRad;
	GeoMath::destination(osg::DegreesToRadians(location.y()),
		osg::DegreesToRadians(location.x()),
		mLabelRotationRad,
		2500.0,
		latRad,
		longRad);

	mGeoPointProj.set(osgEarth::SpatialReference::get("wgs84"),
		osg::RadiansToDegrees(longRad),
		osg::RadiansToDegrees(latRad),
		0,
		osgEarth::ALTMODE_ABSOLUTE);

	mGeoPointLoc.set(osgEarth::SpatialReference::get("wgs84"),
		//location.getSRS(),
		location.x(),
		location.y(),
		0,
		osgEarth::ALTMODE_ABSOLUTE);

	const TextSymbol* ts = getStyle().get<TextSymbol>();
	if (ts)
	{
		mDataLayout->setPixelOffset(ts->pixelOffset().get());
		if (mFollowFixedCourse)
		{
			osg::Vec3d p0, p1;
			mGeoPointLoc.toWorld(p0);
			mGeoPointProj.toWorld(p1);
			mDataLayout->setAnchorPoint(p0);
			mDataLayout->setProjPoint(p1);
		}
	}
}

void RichLabelNode::setDynamic(bool dynamic)
{
	GeoPositionNode::setDynamic(dynamic);
	if (mTextGeometry)
		mTextGeometry->setDataVariance(dynamic ? osg::Object::DYNAMIC : osg::Object::STATIC);
}

Config RichLabelNode::getConfig() const
{
	Config conf("label");
	conf.add("text", mText);
	conf.add("style", mStyle);
	return conf;
}
