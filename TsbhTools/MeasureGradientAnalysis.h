#include "RichLabelNode.h"
#include "TsbhTools.h"

#include <osgEarth/MapNode>
#include <osgEarth/Style>
#include <osgEarth/TextSymbol>
#include <osgEarth/BBoxSymbol>
#include <osgEarth/RectangleNode>
#include <osg/TransferFunction>
#include <QMap>
#include <osg/Drawable>

using namespace osgEarth;

class TsbhTools_EXPORT MeasureGradientAnalysis :public TsbhTools
{
public:
	MeasureGradientAnalysis(osg::Group* group, MapNode* node);
	~MeasureGradientAnalysis();
	void  onLeftButton(GeoPoint* Point);

	void  onDoubleClick(GeoPoint* Point);

	void  onMouseMove(GeoPoint* Point);

	void CreateRectAngle(GeoPoint end);

	void CurRectAngle(GeoPoint p1, GeoPoint p2, GeoPoint p3, GeoPoint p4);//↓→

	osgEarth::RectangleNode::Corner getPointCorner1(const osgEarth::GeoPoint & corner);

	osg::ref_ptr<osg::Vec3Array> getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num);

	void  getInsPoints(MapNode * mapNode, osg::Vec3d start, osg::Vec3d end, int num, osg::ref_ptr<osg::Vec3Array> array);

	void loadContourMap(osg::ref_ptr<osg::Vec3Array> array);

	void loadContourImage(osg::ref_ptr<osg::Vec3Array> array);

	void renderCircleNode(osg::Vec3d  point);

	protected:
	 osg::Drawable* loadArrow();

	 double getAngle(osg::ref_ptr<osg::Vec3Array> array,int i,int j);
	 double getPodu(osg::ref_ptr<osg::Vec3Array> array,int i, int j);
	 double getPoxiang(osg::ref_ptr<osg::Vec3Array> array,int i, int j);
	 void getWidthHeight(osg::ref_ptr<osg::Vec3Array> array, double& width, double& height);
private:
	RectangleNode* m_pRectangleNode;
	osgEarth::Style mRectangleStyle;
	osg::TransferFunction1D* m_tf;
	osg::Vec3d mStartPos;
	osg::Vec3d mEndPos;

	int m_nCut;
	int m_nAddHeight;

	//osg::ref_ptr<osg::Vec3Array> _array;
	//unsigned char* _ptr;
};



