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

class TsbhTools_EXPORT MeasureProfileAnalysis :public TsbhTools
{
public:
	MeasureProfileAnalysis(osg::Group* group, MapNode* node);
	~MeasureProfileAnalysis();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point);

	void  onMouseMove(GeoPoint* Point);

	bool buildModel();
	//osg::Node* createLabel();
	void updateFeatureNode();
	void renderCircleNode(GeoPoint*);

	osg::Geode * lined(osg::Vec3 a, osg::Vec3 b);

	osg::ref_ptr<osg::Vec3Array> getInsPoints(MapNode * mapNode, osg::Vec3 start, osg::Vec3 end, int num);
	osg::Geode * drawEdges(osg::ref_ptr<osg::Vec3Array>  points);
	osg::Geode * createProFileGeode(osg::Vec3 start, osg::Vec3 end,const osg::Vec3Array * insPosArr);
private:

	GeoPoint* position;
	RichLabelNode* mLabelNode;
	osg::ref_ptr<osg::Geometry> mGeometry;
	std::vector<osg::Vec3d> m_vecPoint;
	bool isUpdateFeature;
};



