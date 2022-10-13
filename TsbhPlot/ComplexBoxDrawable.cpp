#include "PolygonBoxDrawble.h"
#include<osg/LineWidth>
#include<osg/BlendFunc>
#include<osg/Texture2D>
#include<osg/Image>
#include<osgDB\ReadFile>
ComplexBoxDrawable::ComplexBoxDrawable(const osg::BoundingBox & box, const osgEarth::BBoxSymbol & bboxSymbol):osg::Geometry(),left(0),top(0)
{
	setUseVertexBufferObjects(true);

	float margin = bboxSymbol.margin().isSet() ? bboxSymbol.margin().value() : 2.f;
	osg::Vec3Array* v = new osg::Vec3Array();
	osg::Vec2Array*coors = new osg::Vec2Array;
	/*if (bboxSymbol.geom().isSet() && bboxSymbol.geom().value() == osgEarth::BBoxSymbol::GEOM_BOX_ORIENTED)
	{
	float h = box.yMax() - box.yMin() + 2.f * margin;
	v->push_back(osg::Vec3(box.xMax() + margin + h / 2.f, box.yMax() + margin - h / 2.f, 0));
	}*/
	v->push_back(osg::Vec3(box.xMax() + margin, box.yMax() + margin, 0));
	coors->push_back(osg::Vec2(1.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - margin, box.yMax() + margin, 0));
	coors->push_back(osg::Vec2(0.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - margin, box.yMin() - margin, 0));
	coors->push_back(osg::Vec2(0.0, 0.0));
	v->push_back(osg::Vec3(box.xMax() + margin, box.yMin() - margin, 0));
	coors->push_back(osg::Vec2(1.0, 0.0));
	setVertexArray(v);
	setTexCoordArray(0, coors);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);


	addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, v->size()));
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;

	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	tex->setDataVariance(osg::Object::DYNAMIC);

	tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	osg::ref_ptr<osg::Image> img = osgDB::readImageFile("resources/textures/info3.png");
	if (img.valid())
	{
		tex->setImage(img);
	}
	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFunc = new osg::BlendFunc;
	pBlendFunc->setSourceAlpha(osg::BlendFunc::SRC_ALPHA);
	pBlendFunc->setDestinationAlpha(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	getOrCreateStateSet()->setAttributeAndModes(pBlendFunc ,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	// Disable culling since this bounding box will eventually be drawn in screen space.
	setCullingActive(false);


}
ComplexBoxDrawable::ComplexBoxDrawable(const osg::BoundingBox & box, float margin) :osg::Geometry(), left(0), top(0)
{
	setUseVertexBufferObjects(true);

	
	osg::Vec3Array* v = new osg::Vec3Array();
	osg::Vec2Array*coors = new osg::Vec2Array;
	/*if (bboxSymbol.geom().isSet() && bboxSymbol.geom().value() == osgEarth::BBoxSymbol::GEOM_BOX_ORIENTED)
	{
	float h = box.yMax() - box.yMin() + 2.f * margin;
	v->push_back(osg::Vec3(box.xMax() + margin + h / 2.f, box.yMax() + margin - h / 2.f, 0));
	}*/
	v->push_back(osg::Vec3(box.xMax() + margin, box.yMax() + margin, 0));
	coors->push_back(osg::Vec2(1.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - margin, box.yMax() + margin, 0));
	coors->push_back(osg::Vec2(0.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - margin, box.yMin() - margin, 0));
	coors->push_back(osg::Vec2(0.0, 0.0));
	v->push_back(osg::Vec3(box.xMax() + margin, box.yMin() - margin, 0));
	coors->push_back(osg::Vec2(1.0, 0.0));
	setVertexArray(v);
	setTexCoordArray(0, coors);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);


	addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, v->size()));
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;

	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	tex->setDataVariance(osg::Object::DYNAMIC);

	tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	osg::ref_ptr<osg::Image> img = osgDB::readImageFile("resources/textures/info3.png");
	if (img.valid())
	{
		tex->setImage(img);
	}
	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFunc = new osg::BlendFunc;
	pBlendFunc->setSourceAlpha(osg::BlendFunc::SRC_ALPHA);
	pBlendFunc->setDestinationAlpha(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	getOrCreateStateSet()->setAttributeAndModes(pBlendFunc, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	// Disable culling since this bounding box will eventually be drawn in screen space.
	setCullingActive(false);


}
ComplexBoxDrawable::ComplexBoxDrawable(const osg::BoundingBox & box, float marginTop, float marginBottom, float marginLeft, float marginRight)
{
	setUseVertexBufferObjects(true);

	osg::Vec3Array* v = new osg::Vec3Array();
	osg::Vec2Array*coors = new osg::Vec2Array;

	v->push_back(osg::Vec3(box.xMax() + marginRight, box.yMax() + marginTop, 0));
	coors->push_back(osg::Vec2(1.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - marginLeft, box.yMax() + marginTop, 0));
	coors->push_back(osg::Vec2(0.0, 1.0));
	v->push_back(osg::Vec3(box.xMin() - marginLeft, box.yMin() - marginBottom, 0));
	coors->push_back(osg::Vec2(0.0, 0.0));
	v->push_back(osg::Vec3(box.xMax() + marginRight, box.yMin() - marginBottom, 0));
	coors->push_back(osg::Vec2(1.0, 0.0));
	setVertexArray(v);
	setTexCoordArray(0, coors);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);


	addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, v->size()));
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	tex->setDataVariance(osg::Object::DYNAMIC);

	tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	osg::ref_ptr<osg::Image> img = osgDB::readImageFile("resources/textures/info3.png");
	if (img.valid())
	{
		tex->setImage(img);
	}
	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFunc = new osg::BlendFunc;
	pBlendFunc->setSourceAlpha(osg::BlendFunc::SRC_ALPHA);
	pBlendFunc->setDestinationAlpha(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	getOrCreateStateSet()->setAttributeAndModes(pBlendFunc);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	// Disable culling since this bounding box will eventually be drawn in screen space.
	setCullingActive(false);
}


