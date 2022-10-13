#ifndef DRAWPOLYGON_H
#define DRAWPOLYGON_H 1

#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
#include <osgEarth/Style>
#include "DrawTool.h"
#include "FeatureEditing.h"

class TsbhPlot_EXPORT DrawPolygonTool : public DrawTool {
public:
    DrawPolygonTool(osgEarth::MapNode* mapNode, osg::Group* drawGroup);

    virtual DrawType getType() { return DRAW_POLYGON; }
	virtual ToolType getToolType() { return POLYGON; }
    virtual void beginDraw(const osg::Vec3d& lla);
    virtual void moveDraw(const osg::Vec3d& lla);
    virtual void endDraw(const osg::Vec3d& lla);
    virtual void resetDraw();
	virtual void remove();
	osg::ref_ptr<osg::Node> draw(std::vector<osg::Vec3d> points);
	std::vector<osg::Vec3d> getPoints();

	void setStyle(osgEarth::Style style);

	NodeInfo getNodeinfos();
private:
    std::vector<osg::Vec3d> _vecPoints;
	std::vector<osg::Vec3d> _temPoint;
    osgEarth::Style _polygonStyle;
    osgEarth::Style _stippleLineStyle;
	ToolProperty*  m_Property = NULL;
    osgEarth::FeatureNode* _stippleFeatureNode;
	osgEarth::FeatureNode* _featureNode;
	osg::ref_ptr<FeatureEditor> _polygonEdit;
};

#endif
