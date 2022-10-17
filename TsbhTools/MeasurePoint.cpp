//////////////////////////////////////////////////////////////////////////
/// Copyright (C), 1998-2016, rytec Corporation. All rights reserved.
/// \brief      三维点位量算图形类

//////////////////////////////////////////////////////////////////////////

#include "MeasurePoint.h"
#include "SignageNode.h"
#include <osgEarth/MapNode>
#include <Transcoder.h>
#include<osgEarth\Registry>



MeasurePoint::MeasurePoint(osg::Group* group, MapNode* node) :TsbhTools()
{
	this->mapNode = node;
	this->mRoot = group;
	position = NULL;
}

MeasurePoint::~MeasurePoint()
{
}


void MeasurePoint::buildModel()
{
	if (!position)
		return;
	
	
	Style labelStyle;
	labelStyle.getOrCreate<osgEarth::TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";  // 暂时
	labelStyle.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color("#034607");
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_LEFT_CENTER;
	labelStyle.getOrCreate<BBoxSymbol>()->fill()->color() = Color(1.0, 1, 1, 0.1);
	labelStyle.getOrCreate<BBoxSymbol>()->border()->color() = Color::Purple;
	labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(2, 100);
	char str[1024];
	sprintf(str, "经度:%f\n\n纬度:%f\n\n高度:%f", position->x(), position->y(), position->z());
	//string str = "dateTime >= '" + start.toString("yyyy/MM/dd hh:mm:ss") + "' and dateTime < '" + end.toString("yyyy/MM/dd hh:mm:ss") + "'";
	std::string text = str;
	mLabelNode = new RichLabelNode(mRoot.get(), (*position), text, labelStyle);
	mLabelNode->setDynamic(true);
	osgEarth::Registry::shaderGenerator().run(mLabelNode);

	mRoot->addChild(mLabelNode);
	//SignageNode* mLabelNode;
	//osgEarth::Style labelStyle;
	//TextSymbol* textSymbol = labelStyle.getOrCreate<osgEarth::TextSymbol>();
	//textSymbol->fill()->color() = osgEarth::Color::White;
	//textSymbol->alignment() = osgEarth::TextSymbol::ALIGN_LEFT_CENTER;
	//textSymbol->pixelOffset() = osg::Vec2s(2, 100);
	//textSymbol->font() = "c:/windows/fonts/simhei.ttf";

	//textSymbol->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;

	//std::stringstream stream;
	//stream.precision(2);
	//stream.setf(std::ios::fixed);
	//stream << "经度:" << position->x() << "\n纬度:" << position->y() << "\n高度:" << position->z();

	//mLabelNode = new SignageNode(mapNode, mRoot, osgEarth::GeoPoint(), stream.str(), labelStyle, SignageNode::Simple);
	//mLabelNode->setDynamic(true);
	//mLabelNode->setLabelType(1);
	//mLabelNode->setStyle2(labelStyle);
	//osgEarth::Registry::shaderGenerator().run(mLabelNode);
	//mLabelNode->setPosition(*position);
	//mRoot->addChild(mLabelNode);
}

void  MeasurePoint::onLeftButton(GeoPoint* Point)
{
	position = Point;
	buildModel();
	pointNum++;
}

void  MeasurePoint::onDoubleClick(GeoPoint* Point)
{
	//onLeftButton(Point);
}

void  MeasurePoint::onMouseMove(GeoPoint* Point)
{
	
}
