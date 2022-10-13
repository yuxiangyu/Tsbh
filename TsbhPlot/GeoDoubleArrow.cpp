#include "GeoDoubleArrow.h"
#include "PlottingMath.h"

using namespace osgEarth;


GeoDoubleArrow::GeoDoubleArrow(osgEarth::MapNode *mapNode, osg::Group *drawGroup)
    : DrawTool(mapNode, drawGroup)
{
    _polygonStyle.getOrCreate<PolygonSymbol>()
        ->fill()
        ->color()
        = Color(Color::Yellow, 0.25);

    LineSymbol* ls = _polygonStyle.getOrCreate<LineSymbol>();
    ls->stroke()->color() = Color::White;
    ls->stroke()->width() = 2.0f;
    ls->stroke()->widthUnits() = osgEarth::Units::PIXELS;

    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->clamping()
        = AltitudeSymbol::CLAMP_TO_TERRAIN;
    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->technique()
        = AltitudeSymbol::TECHNIQUE_DRAPE;
    _polygonStyle.getOrCreate<AltitudeSymbol>()
        ->verticalOffset()
        = 0.1;
}

void GeoDoubleArrow::beginDraw(const osg::Vec3d &lla)
{
    if (_controlPoints.size() >= 4)
        _controlPoints.clear();

     _controlPoints.push_back(osg::Vec2(lla.x(), lla.y()));

    if (_controlPoints.empty() || _controlPoints.size() < 4)
        return;

    _drawParts = calculateParts(_controlPoints);

    if (!_featureNode.valid()) {
         Feature* feature = new Feature(new Polygon, getMapNode()->getMapSRS());
        _featureNode = new FeatureNode(feature, _polygonStyle);
        drawCommand(_featureNode);
    }

    Geometry* geom = _featureNode->getFeature()->getGeometry();
    geom->clear();
    _featureNode->setStyle(_polygonStyle);
    for (auto& n : _drawParts) {
        geom->push_back(osg::Vec3(n.x(), n.y(), 0));
    }
    _featureNode->dirty();
}

void GeoDoubleArrow::moveDraw(const osg::Vec3d &lla)
{
    if (_controlPoints.empty())
        return;
    if (!_featureNode.valid()) {
         Feature* feature = new Feature(new Polygon, getMapNode()->getMapSRS());
        _featureNode = new FeatureNode(feature, _polygonStyle);
        drawCommand(_featureNode);
    }
    if (_featureNode.valid()) {
        std::vector<osg::Vec2> ctrlPts = _controlPoints;
        std::vector<osg::Vec2> drawPts;
        ctrlPts.push_back(osg::Vec2(lla.x(), lla.y()));

        if (ctrlPts.empty() || ctrlPts.size() < 4)
            return;

        drawPts = calculateParts(ctrlPts);

        Geometry* geom = _featureNode->getFeature()->getGeometry();
        geom->clear();
        for (auto& n : drawPts) {
            geom->push_back(osg::Vec3(n.x(), n.y(), 0));
        }
        _featureNode->dirty();
    }
}

void GeoDoubleArrow::endDraw(const osg::Vec3d &lla)
{

}

void GeoDoubleArrow::resetDraw()
{
    _controlPoints.clear();
    _featureNode = NULL;
}

std::vector<osg::Vec2> GeoDoubleArrow::calculateParts(const std::vector<osg::Vec2>& ctrlPts)
{
   
    osg::Vec2 pointU_1(ctrlPts[0]);
    osg::Vec2 pointU_2(ctrlPts[1]);
    osg::Vec2 pointU_3(ctrlPts[2]);
    osg::Vec2 pointU_4(ctrlPts[3]);

    
    osg::Vec2 pointU_C(((pointU_1.x()+pointU_2.x())*5+(pointU_3.x()+pointU_4.x()))/12,((pointU_1.y()+pointU_2.y())*5+(pointU_3.y()+pointU_4.y()))/12);
    
    osg::Vec2 pointC_l_out, pointC_r_out;
    pointC_l_out = Math::calculateIntersectionFromTwoCorner(pointU_1, pointU_4, osg::PI/8, osg::PI/6)[0];
    pointC_r_out = Math::calculateIntersectionFromTwoCorner(pointU_2, pointU_3,osg::PI/8,osg::PI/6)[1];

    
     osg::Vec2 pointC_l_inner, pointC_r_inner;
    pointC_l_inner = Math::calculateIntersectionFromTwoCorner(pointU_C, pointU_4, osg::PI/8, osg::PI/16)[0];
    pointC_r_inner = Math::calculateIntersectionFromTwoCorner(pointU_C,pointU_3,osg::PI/8,osg::PI/16)[1];

    osg::Vec2 v_l_out(pointC_l_out.x()-pointU_4.x(), pointC_l_out.y()-pointU_4.y());
    float d_l_out = sqrtf(v_l_out.x()*v_l_out.x()+v_l_out.y()*v_l_out.y());
    
    osg::Vec2 v_l_out_1(v_l_out.x()/d_l_out, v_l_out.y()/d_l_out);
    osg::Vec2 v_l_inner(pointC_l_inner.x()-pointU_4.x(), pointC_l_inner.y()-pointU_4.y());
    float d_l_inner = sqrtf(v_l_inner.x()*v_l_inner.x()+v_l_inner.y()*v_l_inner.y());
    
    osg::Vec2 v_l_inner_1(v_l_inner.x()/d_l_inner, v_l_inner.y()/d_l_inner);
   
    float ab = 0.25;

    
    float d_l_a = d_l_out < d_l_inner ? d_l_out*ab : d_l_inner*ab;

    osg::Vec2 pointC_l_out_2(v_l_out_1.x()*d_l_a+pointU_4.x(), v_l_out_1.y()*d_l_a+pointU_4.y());
    osg::Vec2 pointC_l_inner_2(v_l_inner_1.x()*d_l_a+pointU_4.x(), v_l_inner_1.y()*d_l_a+pointU_4.y());

   
    osg::Vec2 pointC_l_a_l(pointC_l_out_2.x()*1.5-pointC_l_inner_2.x()*0.5, pointC_l_out_2.y()*1.5-pointC_l_inner_2.y()*0.5);
    
    osg::Vec2 pointC_l_a_r(pointC_l_inner_2.x()*1.5-pointC_l_out_2.x()*0.5, pointC_l_inner_2.y()*1.5-pointC_l_out_2.y()*0.5);

    osg::Vec2 v_r_out(pointC_r_out.x()-pointU_3.x(), pointC_r_out.y()-pointU_3.y());
    float d_r_out = sqrtf(v_r_out.x()*v_r_out.x()+v_r_out.y()*v_r_out.y());
    osg::Vec2  v_r_out_1(v_r_out.x()/d_r_out, v_r_out.y()/d_r_out);

    osg::Vec2 v_r_inner(pointC_r_inner.x()-pointU_3.x(), pointC_r_inner.y()-pointU_3.y());
    float d_r_inner = sqrtf(v_r_inner.x()*v_r_inner.x()+v_r_inner.y()*v_r_inner.y());
    osg::Vec2 v_r_inner_1(v_r_inner.x()/d_r_inner, v_r_inner.y()/d_r_inner);

   
    float d_r_a =  d_r_out < d_r_inner ? d_r_out*ab : d_r_inner*ab;
    osg::Vec2 pointC_r_out_2(v_r_out_1.x()*d_r_a+pointU_3.x(),v_r_out_1.y()*d_r_a+pointU_3.y());
    osg::Vec2 pointC_r_inner_2(v_r_inner_1.x()*d_r_a+pointU_3.x(), v_r_inner_1.y()*d_r_a+pointU_3.y());
    
    osg::Vec2 pointC_r_a_r(pointC_r_out_2.x()*1.5-pointC_r_inner_2.x()*0.5,pointC_r_out_2.y()*1.5-pointC_r_inner_2.y()*0.5);
    
    osg::Vec2 pointC_r_a_l(pointC_r_inner_2.x()*1.5-pointC_r_out_2.x()*0.5,pointC_r_inner_2.y()*1.5-pointC_r_out_2.y()*0.5);

    
    std::vector<osg::Vec2> bezier_in;
    bezier_in.push_back(pointU_1);
    bezier_in.push_back(pointC_l_out);
    bezier_in.push_back(pointC_l_out_2);
    std::vector<osg::Vec2> points_l = Math::createBezier2(bezier_in);

    
    osg::Vec2 v_U_4_3(pointU_3.x()-pointU_4.x(), pointU_3.y()-pointU_4.y());

   
    osg::Vec2 v_U_4_C(pointU_C.x()-pointU_4.x(), pointU_C.y()-pointU_4.y());
   
    float d_U_4_C = sqrtf(v_U_4_C.x()*v_U_4_C.x()+v_U_4_C.y()*v_U_4_C.y());
    osg::Vec2 v_U_3_C(pointU_C.x()-pointU_3.x(), pointU_C.y()-pointU_3.y());
   
    float d_U_3_C = sqrtf(v_U_3_C.x()*v_U_3_C.x()+v_U_3_C.y()*v_U_3_C.y());

    float percent = 0.4;
    osg::Vec2 v_U_4_3_(v_U_4_3.x()*percent, v_U_4_3.y()*percent);
    osg::Vec2 v_U_4_3_l(v_U_4_3_.x()*d_U_4_C/(d_U_4_C+d_U_3_C), v_U_4_3_.y()*d_U_4_C/(d_U_4_C+d_U_3_C));
    osg::Vec2 v_U_4_3_r(v_U_4_3_.x()*d_U_3_C/(d_U_4_C+d_U_3_C),v_U_4_3_.y()*d_U_3_C/(d_U_4_C+d_U_3_C));
  
    osg::Vec2 pointC_c_l(pointU_C.x()-v_U_4_3_l.x(), pointU_C.y()-v_U_4_3_l.y());
   
    osg::Vec2 pointC_c_r(pointU_C.x()+v_U_4_3_r.x(), pointU_C.y()+v_U_4_3_r.y());
   
    std::vector<osg::Vec2> arr{pointC_l_inner_2, pointC_l_inner, pointC_c_l, pointU_C, pointC_c_r, pointC_r_inner, pointC_r_inner_2};
    //TODO
    std::vector<osg::Vec2> points_c = Math::createBezier(arr, 0, 20);

#if 0   //Note by gkk 20220223 ,报“非常量引用只能绑定到左值”错误
    std::vector<osg::Vec2> points_r = Math::createBezier2(std::vector<osg::Vec2>{pointC_r_out_2, pointC_r_out,pointU_2});
#endif

 #if 1  //Add by gkk 20220223，替换上述写法
    std::vector<osg::Vec2> points_tmp = { pointC_r_out_2, pointC_r_out, pointU_2 };
    std::vector<osg::Vec2> points_r = Math::createBezier2(points_tmp);
#endif
   
    std::vector<osg::Vec2> result = points_l;
    result.push_back(pointC_l_a_l);
    result.push_back(pointU_4);
    result.push_back(pointC_l_a_r);
    result.insert(result.end(), points_c.begin(), points_c.end());
    result.push_back(pointC_r_a_l);
    result.push_back(pointU_3);
    result.push_back(pointC_r_a_r);
    result.insert(result.end(), points_r.begin(), points_r.end());


    return result;

//    _drawParts.clear();
//    _drawParts = result;
}
