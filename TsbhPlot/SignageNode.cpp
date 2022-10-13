#include "SignageNode.h"

#include<osgDB/ReadFile>
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
#include "PolygonBoxDrawble.h"
#include <QString>
#include<QTextCodec>
#include<osg/DrawPixels>
#include<osg/BlendFunc>
#include "transcoder.h"

using namespace osgEarth;

//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")// ��ָ���֧��VS����
//#endif



SignageNode::SignageNode(MapNode* mapNode, const GeoPoint& position,  const std::string& text,
	const Style& style, const RichType&rt)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
	, richtype(rt)
	,m_mapnode(mapNode),mID("")
{
	init(style);
	setPosition(position);
	mapNode->addChild(this);
}

SignageNode::SignageNode(MapNode * mapNode, osg::Group* group, const GeoPoint & position, const std::string & text, 
	const Style & style, const RichType & rt)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
	, richtype(rt)
	, m_mapnode(mapNode), mID("")
{
	init(style);
	setPosition(position);
	group->addChild(this);
}

SignageNode::SignageNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
	const TextSymbol* symbol, const RichType&rt)
	: mText(text)
	, mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
	, richtype(rt)
	, m_mapnode(mapNode), mID("")
{
	Style style;
	style.add(const_cast<TextSymbol*>(symbol));

	init(style);
	setPosition(position);
	mapNode->addChild(this);
}

SignageNode::SignageNode(MapNode* mapNode, const Config& conf, const osgDB::Options* dbOptions, const RichType&rt)
	: mLabelRotationRad(0.0f)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL)
	, richtype(rt), mID("")
{
	optional<Style> style;

	conf.get("style", style);
	conf.get("text", mText);

	init(*style);
	mapNode->addChild(this);
}

SignageNode::SignageNode(const SignageNode& rhs, const osg::CopyOp& op)
	: GeoPositionNode(rhs, op)
	, mLabelRotationRad(0.)
	, mFollowFixedCourse(false)
	, mTextGeometry(NULL), mID("")
{
	//nop - unused
}


SignageNode* SignageNode::buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode)
{
	Style labelStyle;
	labelStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";  // ��ʱ
	labelStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_LEFT_CENTER;
	//labelStyle.getOrCreate<TextSymbol>()->size() = 100.0;
	//labelStyle.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1.0, 1.0, 0.1);
	//labelStyle.getOrCreate<BBoxSymbol>()->border()->color() = Color(1.0, 1.0, 1.0, 0.8);
	labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(2, 100);

	auto label = new SignageNode(mapNode, pos, text, labelStyle);
	label->setDynamic(true);
	root->addChild(label);
	return label;
}

void SignageNode::init(const Style& style)
{
	ScreenSpaceLayout::activate(this->getOrCreateStateSet());

	mGeode = new osg::Geode();

	mGeode->setComputeBoundingSphereCallback(new ControlPointCallback());

	getPositionAttitudeTransform()->addChild(mGeode.get());

	osg::StateSet* stateSet = mGeode->getOrCreateStateSet();

	stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);

	setStyle2(style);
}

void SignageNode::setText(const std::string& text)
{
	if (!_dynamic && getNumParents() > 0)
	{
		//    LOG_ERROR(RY_TEXT("�޷��޸ķǶ�̬��ע�ڵ�"));
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
		mText = text;

		mTextGeometry->setText(text, textEncoding);

		mTextGeometry->dirtyGLObjects();

		mGeode->removeChild(mBBoxGeom);

		auto& boundingBox = mTextGeometry->getBoundingBox();

		mBBoxGeom = new SimpleBoxDrawable(boundingBox, 1, 20);

		mGeode->addDrawable(mBBoxGeom);

		mGeode->setCullingActive(false);

		if ( this->mTextGeometry)
		{

			mGeode->removeChild(mTextGeometry);

			mGeode->addDrawable(mTextGeometry);
		}

		Registry::shaderGenerator().run(this, "SignageNode", Registry::stateSetCache());

		updateLayoutData();


	}
}
std::string& SignageNode::getID()
{
	return mID;
}
void SignageNode::setVisiable(bool v)
{
	if (v)
	{
		this->setNodeMask(true);		
	}
	else 
	{
	//if (this->getParent(0) == m_mapnode.get())
	//{
		this->setNodeMask(false);
	//}
	}
}
void SignageNode::setLabelType(int type)
{
	m_type = type;
}
void SignageNode::setStyle2(const Style& style)
{
	if (!_dynamic && getNumParents() > 0)
	{
		//    LOG_ERROR(RY_TEXT("�޷��޸ķǶ�̬��ע�ڵ�"));
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
	//QString text = QString::fromStdString(mText.c_str());
	//std::string text_utf8;
	//	//Transcoder::gb2312ToUtf8(Transcoder::qstr2str(text), text_utf8);
	//Transcoder::gb2312ToUtf8(mText, text_utf8);
	mTextGeometry = (osgText::Text*)AnnotationUtils::createTextDrawable(mText, symbol, osg::Vec3(0, 0, 0));

	mGeode->addDrawable(mTextGeometry);

	auto& boundingBox = mTextGeometry->getBoundingBox();
	if (richtype == Complex2)
	{
		float xc = boundingBox.xMin();
		xc = xc - (boundingBox.xMax() - xc) / 2.0;
		mTextGeometry->setPosition(osg::Vec3(xc, 0, 0));
	}
	auto&box = mTextGeometry->getBoundingBox();
	if (mTextGeometry)
	{
		switch (richtype)
		{
		case Simple:
			mBBoxGeom = new SimpleBoxDrawable(boundingBox,m_type,20);
			break;
		case Complex1:

		{
			mBBoxGeom = new PolygonBoxDrawable(boundingBox,20);
			//mBBoxGeom = osg::createTexturedQuadGeometry(boundingBox.center(), osg::Vec3((box.xMax() - box.xMin()) + 40, 0, 0), osg::Vec3(0,  (box.yMax() - box.yMin()) + 40,0));
			
		
			break;
		}
		break;
		case Complex2:
		{
			mBBoxGeom = new ComplexBoxDrawable(box,20);
			
		}
		break;
		default:
			mBBoxGeom = new SimpleBoxDrawable(boundingBox,20);
			break;
		}

		mGeode->addDrawable(mBBoxGeom);
	}
	mGeode->setCullingActive(false);

	//applyStyle(mStyle);
	//setLightingIfNotSet(false);
	Registry::shaderGenerator().run(this, "SignageNode", Registry::stateSetCache());
	updateLayoutData();

	if (mTextGeometry)
	{
		if (this->mTextGeometry)
		{

			mGeode->removeChild(mTextGeometry);

			mGeode->addDrawable(mTextGeometry);
			updateLayoutData();
		}
		auto&box2 = mTextGeometry->getBoundingBox();
		auto margin = 2.0f;
		osg::ref_ptr<osg::Geometry>(geometry_line) = new osg::Geometry();
		geometry_line->setUseVertexBufferObjects(true);

		osg::Vec3 anchor(-mDataLayout->getPixelOffset().r(), -mDataLayout->getPixelOffset().g(), 0.0);
		osg::Vec3 left_top(box2.xMin() - margin, box2.yMax() + margin, box2.zMax());
		osg::Vec3 left_bottom(box2.xMin() - margin, box2.yMin() - margin, box2.zMax());
		osg::Vec3 right_top(box2.xMax() + margin, box2.yMax() + margin, box2.zMax());
		osg::Vec3 right_bottom(box2.xMax() + margin, box2.yMin() - margin, box2.zMax());
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
		proj = (right_bottom + left_bottom) / 4.0;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		v->push_back(osg::Vec3(-mDataLayout->getPixelOffset().r(), -mDataLayout->getPixelOffset().g(), 0.0));
		v->push_back(proj);
		geometry_line->setVertexArray(v);
		if (v->getVertexBufferObject())
			v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);

		auto c = new osg::Vec4Array();
		c->push_back(osg::Vec4(1,1,1,1));
		
		
		geometry_line->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2.0f));
		geometry_line->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, v->size()));
		geometry_line->setColorArray(c);
		geometry_line->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
		//geometry_line->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

		

		/*geometry_line->setUseDisplayList(false);*/
		// add the static "isText=true" uniform; this is a hint for the annotation shaders
		// if they get installed.
		static osg::ref_ptr<osg::Uniform> s_isTextUniform = new osg::Uniform();
		s_isTextUniform->set(false);
		getOrCreateStateSet()->addUniform(s_isTextUniform.get());
		getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		// Disable culling since this bounding box will eventually be drawn in screen space.
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
		setCullingActive(false);
#endif
		if (m_type == 1) {
			mGeode->addDrawable(geometry_line);
		}
	}
	dirty();

}

void SignageNode::dirty()
{
	GeoPositionNode::dirty();
	updateLayoutData();
}

void SignageNode::setPriority(float value)
{
	GeoPositionNode::setPriority(value);
	updateLayoutData();
}

void SignageNode::updateLayoutData()
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

void SignageNode::setDynamic(bool dynamic)
{
	GeoPositionNode::setDynamic(dynamic);
	if (mTextGeometry)
		mTextGeometry->setDataVariance(dynamic ? osg::Object::DYNAMIC : osg::Object::STATIC);
}

Config SignageNode::getConfig() const
{
	Config conf("label");
	conf.add("text", mText);
	conf.add("style", mStyle);
	return conf;
}

void SignageNode::updatePosition(double x, double y, double z)
{
	GeoPoint pos(m_mapnode->getMapSRS(), x, y, z);

	setPosition(pos);
}

void SignageNode::remove()
{
	m_mapnode->removeChild(this);
}
