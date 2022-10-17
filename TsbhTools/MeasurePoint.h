//////////////////////////////////////////////////////////////////////////
/// Copyright (C), 1998-2016, rytec Corporation. All rights reserved.
/// \brief      三维点位量算图形类
//////////////////////////////////////////////////////////////////////////


#include "RichLabelNode.h"
#include "TsbhTools.h"

#include <osgEarth/MapNode>
#include <osgEarth/Style>
#include <osgEarth/TextSymbol>
#include <osgEarth/BBoxSymbol>



using namespace osgEarth;

class TsbhTools_EXPORT MeasurePoint :public TsbhTools
{
public:
	MeasurePoint(osg::Group* group, MapNode* node);
	~MeasurePoint();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point);

	void  onMouseMove(GeoPoint* Point);

	void buildModel();
	void setText(std::string text) {
		m_text = text;
	}
	//osg::Node* createLabel();
	//void renderCircleNode(GeoPoint*);

private:

	GeoPoint* position;
	RichLabelNode* mLabelNode;
    std::string m_text;
	
};



