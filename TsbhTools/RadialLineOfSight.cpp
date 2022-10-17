/* -*-c++-*- */
/* osgEarth - Geospatial SDK for OpenSceneGraph
* Copyright 2020 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "RadialLineOfSight.h"
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/GLUtils>
#include <osgEarth/Feature>
#include <osgEarth/FeatureNode>
#include <osg/CullFace>

using namespace osgEarth;
using namespace osgEarth::Contrib;


    osg::Vec3d getNodeCenter(osg::Node* node)
    {
        osg::NodePathList nodePaths = node->getParentalNodePaths();
        if ( nodePaths.empty() )
            return node->getBound().center();

        osg::NodePath path = nodePaths[0];

        osg::Matrixd localToWorld = osg::computeLocalToWorld( path );
        osg::Vec3d center = osg::Vec3d(0,0,0) * localToWorld;

        // if the tether node is a MT, we are set. If it's not, we need to get the
        // local bound and add its translation to the localToWorld. We cannot just use
        // the bounds directly because they are single precision (unless you built OSG
        // with double-precision bounding spheres, which you probably did not :)
        if ( !dynamic_cast<osg::MatrixTransform*>( node ) )
        {
            const osg::BoundingSphere& bs = node->getBound();
            center += bs.center();
        }   
        return center;
    }


//------------------------------------------------------------------------


    class RadialLineOfSightNodeTerrainChangedCallback2 : public osgEarth::TerrainCallback
    {
    public:
        RadialLineOfSightNodeTerrainChangedCallback2( RadialLineOfSightNode2* los ):
          _los(los)
        {
        }

        virtual void onTileUpdate(const osgEarth::TileKey& tileKey, osg::Node* graph, TerrainCallbackContext& )
        {
            _los->terrainChanged( tileKey, graph );
        }

    private:
        RadialLineOfSightNode2* _los;
    };



RadialLineOfSightNode2::RadialLineOfSightNode2( MapNode* mapNode):
_mapNode( mapNode ),
_numSpokes(10),
_radius(500),
//_center(0,0,0),
_goodColor(0.0f, 1.0f, 0.0f, 0.5f),
_badColor(1.0f, 0.0f, 0.0f, 0.5f),
_outlineColor( 1.0f, 1.0f, 1.0f, 1.0f),
_displayMode( LineOfSight::MODE_SPLIT ),
_terrainOnly( false )
{
    //compute(getNode());
    _terrainChangedCallback = new RadialLineOfSightNodeTerrainChangedCallback2( this );
    _mapNode->getTerrain()->addTerrainCallback( _terrainChangedCallback.get() );        
    setNumChildrenRequiringUpdateTraversal( 1 );
}

RadialLineOfSightNode2::~RadialLineOfSightNode2()
{
    setMapNode( 0L );
}

void
RadialLineOfSightNode2::setMapNode( MapNode* mapNode )
{
    MapNode* oldMapNode = getMapNode();

    if ( oldMapNode != mapNode )
    {
        if ( oldMapNode )
        {
            if ( _terrainChangedCallback.valid() )
            {
                oldMapNode->getTerrain()->removeTerrainCallback( _terrainChangedCallback.get() );
            }
        }

        _mapNode = mapNode;

        if ( _mapNode.valid() && _terrainChangedCallback.valid() )
        {
            _mapNode->getTerrain()->addTerrainCallback( _terrainChangedCallback.get() );
        }

        compute( getNode() );
    }
}


double
RadialLineOfSightNode2::getRadius() const
{
    return _radius;
}

void
RadialLineOfSightNode2::setRadius(double radius)
{
    if (_radius != radius)
    {
        _radius = osg::clampAbove(radius, 1.0);
        compute(getNode());
    }
}

int
RadialLineOfSightNode2::getNumSpokes() const
{
    return _numSpokes;
}

void RadialLineOfSightNode2::setNumSpokes(int numSpokes)
{
    if (numSpokes != _numSpokes)
    {
        _numSpokes = osg::clampAbove(numSpokes, 1);
        compute(getNode());
    }
}

const osg::Vec3d&
RadialLineOfSightNode2::getCenterWorld() const
{
    return _centerWorld;
}



const GeoPoint&
RadialLineOfSightNode2::getCenter() const
{
    return _center;
}

void
RadialLineOfSightNode2::setCenter(const GeoPoint& center)
{
    if (_center != center)
    {
        _center = center;
		//if(_radius>500)
       // compute(getNode());
    }
}

bool
RadialLineOfSightNode2::getTerrainOnly() const
{
    return _terrainOnly;
}

void RadialLineOfSightNode2::setTerrainOnly( bool terrainOnly )
{
    if (_terrainOnly != terrainOnly)
    {
        _terrainOnly = terrainOnly;
        compute(getNode());
    }
}

osg::Node*
RadialLineOfSightNode2::getNode()
{
    if (_terrainOnly && getMapNode()) return getMapNode()->getTerrainEngine();
    return _mapNode.get();
}


void
RadialLineOfSightNode2::terrainChanged( const osgEarth::TileKey& tileKey, osg::Node* terrain )
{
    OE_DEBUG << "RadialLineOfSightNode::terrainChanged" << std::endl;
    compute( getNode() );    
}

void
RadialLineOfSightNode2::compute(osg::Node* node )
{
   
        compute_fill( node );
   
}

/*
void
RadialLineOfSightNode::compute_line(osg::Node* node)
{    
    if ( !getMapNode() )
        return;

    GeoPoint centerMap;
    _center.transform( getMapNode()->getMapSRS(), centerMap );
    centerMap.toWorld( _centerWorld, getMapNode()->getTerrain() );

    bool isProjected = getMapNode()->getMapSRS()->isProjected();
    osg::Vec3d up = isProjected ? osg::Vec3d(0,0,1) : osg::Vec3d(_centerWorld);
    up.normalize();

    //Get the "side" vector
    osg::Vec3d side = isProjected ? osg::Vec3d(1,0,0) : up ^ osg::Vec3d(0,0,1);

    //Get the number of spokes
    double delta = osg::PI * 2.0 / (double)_numSpokes;
    
    osg::Geometry* geometry = new osg::Geometry;
    geometry->setUseVertexBufferObjects(true);

    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->reserve(_numSpokes * 5);
    geometry->setVertexArray( verts );

    osg::Vec4Array* colors = new osg::Vec4Array(osg::Array::BIND_PER_VERTEX);
    colors->reserve( _numSpokes * 5 );

    geometry->setColorArray( colors );

    osg::Vec3d previousEnd;
    osg::Vec3d firstEnd;

    osg::ref_ptr<osgUtil::IntersectorGroup> ivGroup = new osgUtil::IntersectorGroup();

    for (unsigned int i = 0; i < (unsigned int)_numSpokes; i++)
    {
        double angle = delta * (double)i;
        osg::Quat quat(angle, up );
        osg::Vec3d spoke = quat * (side * _radius);
        osg::Vec3d end = _centerWorld + spoke;
        osg::ref_ptr<osgUtil::LineSegmentIntersector> dplsi = new osgUtil::LineSegmentIntersector( _centerWorld, end );
        ivGroup->addIntersector( dplsi.get() );
    }

    osgUtil::IntersectionVisitor iv;
    iv.setIntersector( ivGroup.get() );

    node->accept( iv );

    for (unsigned int i = 0; i < (unsigned int)_numSpokes; i++)
    {
        osgUtil::LineSegmentIntersector* los = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[i].get());
        if ( !los )
            continue;

        osgUtil::LineSegmentIntersector::Intersections& hits = los->getIntersections();

        osg::Vec3d start = los->getStart();
        osg::Vec3d end = los->getEnd();

        osg::Vec3d hit;
        bool hasLOS = hits.empty();
        if (!hasLOS)
        {
            hit = hits.begin()->getWorldIntersectPoint();
        }

        if (hasLOS)
        {
            verts->push_back( start - _centerWorld );
            verts->push_back( end - _centerWorld );
            colors->push_back( _goodColor );
            colors->push_back( _goodColor );
        }
        else
        {
            if (_displayMode == LineOfSight::MODE_SPLIT)
            {
                verts->push_back( start - _centerWorld );
                verts->push_back( hit - _centerWorld  );
                colors->push_back( _goodColor );
                colors->push_back( _goodColor );

                verts->push_back( hit - _centerWorld );
                verts->push_back( end - _centerWorld );
                colors->push_back( _badColor );
                colors->push_back( _badColor );
            }
            else if (_displayMode == LineOfSight::MODE_SINGLE)
            {
                verts->push_back( start - _centerWorld );
                verts->push_back( end - _centerWorld );
                colors->push_back( _badColor );                                
                colors->push_back( _badColor );                
            }
        }


        if (i > 0)
        {
            verts->push_back( end - _centerWorld );
            verts->push_back( previousEnd - _centerWorld );
            colors->push_back( _outlineColor );
            colors->push_back( _outlineColor );
        }
        else
        {
            firstEnd = end;
        }

        previousEnd = end;
    }


    //Add the last outside of circle
    verts->push_back( firstEnd - _centerWorld );
    verts->push_back( previousEnd - _centerWorld );
    colors->push_back( osg::Vec4(1,1,1,1));
    colors->push_back( osg::Vec4(1,1,1,1));

    geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, verts->size()));

    osg::Geode* geode = new osg::Geode();
    geode->addDrawable( geometry );

    GLUtils::setLighting(getOrCreateStateSet(), osg::StateAttribute::OFF);

    osg::MatrixTransform* mt = new osg::MatrixTransform;
    mt->setMatrix(osg::Matrixd::translate(_centerWorld));
    mt->addChild(geode);
    
    //Remove all the children
    removeChildren(0, getNumChildren());
    addChild( mt );  

    for( LOSChangedCallbackList::iterator i = _changedCallbacks.begin(); i != _changedCallbacks.end(); i++ )
    {
        i->get()->onChanged();
    }	
}
*/
void
RadialLineOfSightNode2::compute_fill(osg::Node* node)
{
    if ( !getMapNode() )
        return;
	GeoPoint centerMap;
	_center.transform(getMapNode()->getMapSRS(), centerMap);
	centerMap.toWorld(_centerWorld, getMapNode()->getTerrain());

	bool isProjected = getMapNode()->getMapSRS()->isProjected();
	osg::Vec3d up = isProjected ? osg::Vec3d(0, 0, 1) : osg::Vec3d(_centerWorld);
	up.normalize();

	//Get the "side" vector
	osg::Vec3d side = isProjected ? osg::Vec3d(1, 0, 0) : up ^ osg::Vec3d(0, 0, 1);
    

    //Get the number of spokes
    double delta = osg::PI * 2.0 / (double)_numSpokes;

    osg::ref_ptr<osgUtil::IntersectorGroup> ivGroup = new osgUtil::IntersectorGroup();

    for (unsigned int i = 0; i < (unsigned int)_numSpokes; i++)
    {
        double angle = delta * (double)i;
        osg::Quat quat(angle, up );
        osg::Vec3d spoke = quat * (side * _radius);
        osg::Vec3d end = _centerWorld + spoke;        
        osg::ref_ptr<osgUtil::LineSegmentIntersector> dplsi = new osgUtil::LineSegmentIntersector( _centerWorld, end );
        if (dplsi)
            ivGroup->addIntersector( dplsi.get() );
    }

    osgUtil::IntersectionVisitor iv;
    iv.setIntersector( ivGroup.get() );

    node->accept( iv );

	insideResults.clear();
	outerResults.clear();
	for (unsigned int i = 0; i < (unsigned int)_numSpokes; i++)
	{
		osgUtil::LineSegmentIntersector* los = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[i].get());
		if (!los)
			continue;

		osgUtil::LineSegmentIntersector::Intersections& hits = los->getIntersections();

		osg::Vec3d start = los->getStart();
		osg::Vec3d end = los->getEnd();

		osg::Vec3d hit;
		bool hasLOS = hits.empty();
		if (!hasLOS)
		{
			hit = hits.begin()->getWorldIntersectPoint();
		}

		if (hasLOS)
		{
			//verts->push_back(start - _centerWorld);
			GeoPoint pc0;
			pc0.fromWorld(getMapNode()->getMapSRS(), start);
			//verts->push_back(end - _centerWorld);
			GeoPoint pc1;
			pc1.fromWorld(getMapNode()->getMapSRS(), end);
			//colors->push_back(_goodColor);
			//colors->push_back(_goodColor);
			osg::Vec3d center1 = (start + end) / 2;
			GeoPoint pc;
			pc.fromWorld(getMapNode()->getMapSRS(), center1);
			//避免0没有相反数
			insideResults.push_back(PointVisResult(pc0, -(i+1), true));
			insideResults.push_back(PointVisResult(pc, i+1, true));
			outerResults.push_back(PointVisResult(pc, -(i+1), true));
			outerResults.push_back(PointVisResult(pc1, i+1, true));
		}
		else
		{
			if (_displayMode == LineOfSight::MODE_SPLIT)
			{
				GeoPoint pc0;
				pc0.fromWorld(getMapNode()->getMapSRS(), start);
				GeoPoint pc1;
				pc1.fromWorld(getMapNode()->getMapSRS(), end );
				GeoPoint pc;
				pc.fromWorld(getMapNode()->getMapSRS(), hit);
				insideResults.push_back(PointVisResult(pc0, -(i + 1), true));
				insideResults.push_back(PointVisResult(pc, i+1, true));
				//内部可见，外部不可见
				outerResults.push_back(PointVisResult(pc, -(i + 1), false));
				outerResults.push_back(PointVisResult(pc1, i+1, false));
			}
			else if (_displayMode == LineOfSight::MODE_SINGLE)
			{
				GeoPoint pc0;
				pc0.fromWorld(getMapNode()->getMapSRS(), start);
				GeoPoint pc1;
				pc1.fromWorld(getMapNode()->getMapSRS(), end );
				osg::Vec3d center1 = (start + end) / 2 ;
				GeoPoint pc;
				pc.fromWorld(getMapNode()->getMapSRS(), center1);

				insideResults.push_back(PointVisResult(pc0, -(i + 1), false));
				insideResults.push_back(PointVisResult(pc, i+1, false));
				//全部不可见
				outerResults.push_back(PointVisResult(pc, -(i + 1), false));
				outerResults.push_back(PointVisResult(pc1, i+1, false));
			}
		}
	}
    
	
	//GLUtils::setLighting(getOrCreateStateSet(), osg::StateAttribute::OFF);
	//getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//开始绘制
	 removeChildren(0, getNumChildren());
	/* for (int i = 1; i < (unsigned int)_numSpokes+1; i++)
	 {
		 renderInsideGeometry(i);
		 renderOuterGeometry(i);
	}*/
	 renderGeometry();


      

    for( LOSChangedCallbackList::iterator i = _changedCallbacks.begin(); i != _changedCallbacks.end(); i++ )
    {
        i->get()->onChanged();
    }	
}

void RadialLineOfSightNode2::renderGeometry()
{
	osg::Geometry* geometry = new osg::Geometry;
	geometry->setUseVertexBufferObjects(true);

	osg::Vec3Array* verts = new osg::Vec3Array();
	verts->reserve(_numSpokes * 3);
	geometry->setVertexArray(verts);

	//增加颜色
	osg::Vec4Array* colors = new osg::Vec4Array(osg::Array::BIND_PER_VERTEX);
	colors->reserve(_numSpokes * 3);
	geometry->setColorArray(colors);
	//增加顶点
	//verts->push_back(_centerWorld);

	for (int i = 1; i < _numSpokes+1; i++)
	{
		for(int j=0;j<insideResults.size();j++)
		{ 
		if (insideResults[j].lineId == -(i))
		{
			//GeoPoint pc = insideResults[i].p;
			//osg::Vec3d dd;
			//pc.toWorld(dd);
			verts->push_back(_centerWorld);
			if (insideResults[j].bVis)
				colors->push_back(_goodColor);
			else
				colors->push_back(_badColor);
			break;
		}
		}
	}

	for (int i = 1; i < _numSpokes + 1; i++)
	{
		for (int j = 0; j < insideResults.size(); j++)
		{
			if (insideResults[j].lineId == i)
			{
				GeoPoint pc = insideResults[j].p;
				osg::Vec3d dd;
				pc.toWorld(dd);
				verts->push_back(dd);
				if (insideResults[j].bVis)
					colors->push_back(_goodColor);
				else
					colors->push_back(_badColor);
				break;
			}
		}
	}

	for (int i = 1; i < _numSpokes + 1; i++)
	{
		for (int j = 0; j < outerResults.size(); j++)
		{
			if (outerResults[j].lineId == i)
			{
				GeoPoint pc = outerResults[j].p;
				osg::Vec3d dd;
				pc.toWorld(dd);
				verts->push_back(dd);
				if (outerResults[j].bVis)
					colors->push_back(_goodColor);
				else
					colors->push_back(_badColor);
				break;
			}
		}
	}
	
	for (int i = 0; i < _numSpokes; i++)
	{
		int j = i + 1;
		if (j >= _numSpokes)
			j = 0;
		osg::DrawElementsUInt* pyramidFace = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		pyramidFace->push_back(i);
		pyramidFace->push_back(_numSpokes+i);
		pyramidFace->push_back(_numSpokes + j);
		geometry->addPrimitiveSet(pyramidFace);

		osg::DrawElementsUInt* pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
		//pyramidFace->push_back(i);
		pyramidBase->push_back(_numSpokes + i);
		pyramidBase->push_back(2*_numSpokes + i);
		pyramidBase->push_back(2 * _numSpokes + j);
		pyramidBase->push_back(_numSpokes + j);
		geometry->addPrimitiveSet(pyramidBase);
	}
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(geometry);

	addChild(geode);
	GLUtils::setLighting(getOrCreateStateSet(), osg::StateAttribute::OFF);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::ref_ptr<osg::CullFace> cullface = new osg::CullFace(osg::CullFace::BACK);
	getOrCreateStateSet()->setAttribute(cullface.get());
	getOrCreateStateSet()->setMode(GL_CULL_FACE, 0);
}

void RadialLineOfSightNode2::renderInsideGeometry(int index)
{
	PointVisResult p1, p2,p3,p4;
	int visNum=0;
	int index2 = index + 1;
	if (index >= insideResults.size()/2)
		index2 = 1;
	for (int i = 0; i < insideResults.size(); i++)
	{
		if (insideResults[i].lineId == -(index))
			p1 = insideResults[i];
		if (insideResults[i].lineId == index)
			p2 = insideResults[i];
		if (insideResults[i].lineId == index2)
			p3 = insideResults[i];
		if (insideResults[i].lineId == -(index2))
			p4 = insideResults[i];
	}
	if (p1.bVis)
		visNum++;
	if (p2.bVis)
		visNum++;
	if (p3.bVis)
		visNum++;
	if (p4.bVis)
		visNum++;

	//绘制出多边形
	Style style1;
//	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	if(visNum>=2)
	style1.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(0, 1, 0, 0.5);// osgEarth::Color::Yellow;
	else
		style1.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(1, 0, 0, 0.5);// osgEarth::Color::Yellow;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	/*LineSymbol* ls = style1.getOrCreate<LineSymbol>();
	if (visNum >= 2)
	ls->stroke()->color() = osg::Vec4f(0, 1, 0, 0.0);
	else
		ls->stroke()->color() = osg::Vec4f(1, 0, 0, 0.0);
	ls->stroke()->width() = 1.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 5;*/

	Polygon* geom = new Polygon;
	geom->push_back(p1.p.x(), p1.p.y(), p1.p.alt());
	geom->push_back(p2.p.x(), p2.p.y(), p2.p.alt());
	geom->push_back(p3.p.x(), p3.p.y(), p3.p.alt());
	//构建feature
	Feature* feature = new Feature(
		geom,
		getMapNode()->getMapSRS());

	FeatureNode* featureNode = new FeatureNode(
		feature, style1);
	//featureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	addChild(featureNode);


//Geometry* geom = featureNode->getFeature()->getGeometry();
//geom->clear();
//featureNode->setStyle(style1);

if (p1.p.x() == 0 || p2.p.x() == 0 || p3.p.x() == 0)
{
	std::string ss = "";
}
	
	//geom->push_back(p4.p.x(), p4.p.y(), p4.p.alt());
//featureNode->dirty();

}
//获取外环的多边形

//获取多边形
void RadialLineOfSightNode2::renderOuterGeometry(int index)
{
	PointVisResult p1, p2, p3, p4;

	int visNum = 0;
	int index2 = index + 1;
	if (index2 >= outerResults.size()/2 + 1)
		index2 = 1;
	for (int i = 0; i < outerResults.size(); i++)
	{
		if (outerResults[i].lineId == -(index))
			p1 = outerResults[i];
		if (outerResults[i].lineId == index)
			p2 = outerResults[i];
		if (outerResults[i].lineId == index2)
			p3 = outerResults[i];
		if (outerResults[i].lineId == -(index2))
			p4 = outerResults[i];
	}
	if (p1.bVis)
		visNum++;
	if (p2.bVis)
		visNum++;
	if (p3.bVis)
		visNum++;
	if (p4.bVis)
		visNum++;

	//绘制出多边形
	Style style1;
	//	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	if (visNum >= 2)
		style1.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(0, 1, 0, 0.5);// osgEarth::Color::Yellow;
	else
		style1.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osg::Vec4f(1, 0, 0, 0.5);// osgEarth::Color::Yellow;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	style1.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0.1;

	//LineSymbol* ls = style1.getOrCreate<LineSymbol>();
	//if (visNum >= 2)
	//	ls->stroke()->color() = osg::Vec4f(0, 1, 0, 0.0);
	//else
	//	ls->stroke()->color() = osg::Vec4f(1, 0, 0, 0.0);
	//ls->stroke()->width() = 1.0f;
	//ls->stroke()->widthUnits() = Units::PIXELS;
	//ls->tessellation() = 5;

	//构建feature
	

	Polygon* geom = new Polygon;
	//polygon1->push_back
		geom->push_back(p1.p.x(), p1.p.y(), p1.p.alt());
	geom->push_back(p2.p.x(), p2.p.y(), p2.p.alt());
	geom->push_back(p3.p.x(), p3.p.y(), p3.p.alt());
	geom->push_back(p4.p.x(), p4.p.y(), p4.p.alt());
	Feature* feature = new Feature(
		geom,
		getMapNode()->getMapSRS());
	FeatureNode* featureNode = new FeatureNode(
		feature, style1);
	//featureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	addChild(featureNode);


//	Geometry* geom = featureNode->getFeature()->getGeometry();
	//geom->clear();
	//featureNode->setStyle(style1);
	/*if (p1.p.x() == 0 || p2.p.x() == 0 || p3.p.x() == 0|| p4.p.x() == 0)
	{
		std::string ss = "";
	}*/
	
	//geom->push_back(p1.p.x(), p1.p.y(), p1.p.alt());
//	featureNode->dirty();

}

void
RadialLineOfSightNode2::setGoodColor( const osg::Vec4f &color )
{
    if (_goodColor != color)
    {
        _goodColor = color;
        compute(getNode());
    }
}

const osg::Vec4f&
RadialLineOfSightNode2::getGoodColor() const
{
    return _goodColor;
}

void
RadialLineOfSightNode2::setBadColor( const osg::Vec4f &color )
{
    if (_badColor != color)
    {
        _badColor = color;
        compute(getNode());
    }
}

const osg::Vec4f&
RadialLineOfSightNode2::getBadColor() const
{
    return _badColor;
}

void
RadialLineOfSightNode2::setOutlineColor( const osg::Vec4f &color )
{
    if (_outlineColor != color)
    {
        _outlineColor = color;
        compute(getNode());
    }
}

const osg::Vec4f&
RadialLineOfSightNode2::getOutlineColor() const
{
    return _outlineColor;
}

LineOfSight::DisplayMode
RadialLineOfSightNode2::getDisplayMode() const
{
    return _displayMode;
}

void
RadialLineOfSightNode2::setDisplayMode( LineOfSight::DisplayMode displayMode )
{
    if (_displayMode != displayMode)
    {
        _displayMode = displayMode;
        compute(getNode());
    }
}

void
RadialLineOfSightNode2::addChangedCallback( LOSChangedCallback* callback )
{
    _changedCallbacks.push_back( callback );
}

void
RadialLineOfSightNode2::removeChangedCallback( LOSChangedCallback* callback )
{
    LOSChangedCallbackList::iterator i = std::find( _changedCallbacks.begin(), _changedCallbacks.end(), callback);
    if (i != _changedCallbacks.end())
    {
        _changedCallbacks.erase( i );
    }    
}



/**********************************************************************/
RadialLineOfSightTether2::RadialLineOfSightTether2(osg::Node* node):
_node(node)
{
}

void 
RadialLineOfSightTether2::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
    {
        RadialLineOfSightNode2* los = static_cast<RadialLineOfSightNode2*>(node);

        if ( los->getMapNode() )
        {
            osg::Vec3d worldCenter = getNodeCenter( _node.get() );

            //Convert center to mappoint since that is what LOS expects
            GeoPoint mapCenter;
            mapCenter.fromWorld( los->getMapNode()->getMapSRS(), worldCenter );

            los->setCenter( mapCenter ); //mapCenter.vec3d() );
        }
    }
    traverse(node, nv);
}


/**********************************************************************/



    struct RadialUpdateDraggersCallback2 : public LOSChangedCallback
    {
    public:
        RadialUpdateDraggersCallback2( RadialLineOfSightEditor2 * editor ):
          _editor( editor )
        {

        }
        virtual void onChanged()
        {
            _editor->updateDraggers();
        }

        RadialLineOfSightEditor2 *_editor;
    };

        
    class RadialLOSDraggerCallback2 : public osgEarth::Dragger::PositionChangedCallback
    {
    public:
        RadialLOSDraggerCallback2(RadialLineOfSightNode2* los):
            _los(los), _start(true)
          {
          }

          virtual void onPositionChanged(const osgEarth::Dragger* sender, const osgEarth::GeoPoint& position)
          {
              _los->setCenter( position );

              //GeoPoint location(position);
              //if (_los->getAltitudeMode() == ALTMODE_RELATIVE)
              //{
              //    double z = _los->getCenter().z();
              //    location.z() = z;
              //}                  
              //_los->setCenter( location.vec3d() );
          }

          RadialLineOfSightNode2* _los;
          bool _start;
    };




/**********************************************************************/

RadialLineOfSightEditor2::RadialLineOfSightEditor2(RadialLineOfSightNode2* los):
_los(los)
{

    _dragger  = new osgEarth::SphereDragger(_los->getMapNode());
    _dragger->addPositionChangedCallback(new RadialLOSDraggerCallback2(_los.get() ) );    
    static_cast<osgEarth::SphereDragger*>(_dragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_dragger);    

    _callback = new RadialUpdateDraggersCallback2( this );
    _los->addChangedCallback( _callback.get() );

    updateDraggers();
}

RadialLineOfSightEditor2::~RadialLineOfSightEditor2()
{
    _los->removeChangedCallback( _callback.get() );
}



void
RadialLineOfSightEditor2::updateDraggers()
{
    if ( _los->getMapNode() )
    {
        osg::Vec3d center = _los->getCenterWorld();             
        GeoPoint centerMap;
        centerMap.fromWorld(_los->getMapNode()->getMapSRS(), center);    
        _dragger->setPosition(centerMap, false);        
    }
}
