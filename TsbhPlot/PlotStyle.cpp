#include "PlotStyle.h"
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osgEarth/PlaceNode>

#include <osgEarth/ModelNode>
using namespace osgEarth;

Style PlotStyle::buildStyle2(osg::Vec4 &color, osg::Vec4 &color2, float width, bool is)
{

	
	Style style;
	//线条
	LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
	ls->stroke()->color() = color;
	ls->stroke()->width() = 2.0f;
	ls->stroke()->smooth() = true;
	ls->stroke()->widthUnits() = Units::PIXELS;
	
	//间隔
	//ls->tessellationSize() = 1;
	//高度
	AltitudeSymbol* as = style.getOrCreate<AltitudeSymbol>();
	as->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;
//	as->technique() = AltitudeSymbol::TECHNIQUE_GPU;

	//RenderSymbol* rs = style.getOrCreateSymbol<RenderSymbol>();
	//rs->depthOffset()->enabled() = true;
	//rs->depthOffset()->minBias() = 1000;
	style.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;

	if (is)
	{
		style.getOrCreate<PolygonSymbol>()->fill()->color() = color2;
//		style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//		style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
	}


	//点符号
	PointSymbol* ps = style.getOrCreate<PointSymbol>();
	//点大小
	ps->size() = (int)width;
	//填充
	ps->fill()->color() = color;


	return style;
}
