/* -*-c++-*- */
/* osgEarth - Geospatial SDK for OpenSceneGraph
* Copyright 2019 Pelican Mapping
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

#include "AnnotationEditing.h"
#include <osgEarth/GeoMath>
#include <osg/io_utils>
#include <osgEarth/Draggers>
#include <osgEarth/GeoPositionNode>
#include "ToolProperty.h"
#include <qdebug.h>

using namespace osgEarth;
/**********************************************************************/
class DraggerCallback : public osgEarth::Dragger::PositionChangedCallback
{
public:
	DraggerCallback(osgEarth::GeoPositionNode* node, GeoPositionNodeEditor* editor, osg::ref_ptr<osg::Node>modelNode) :
		_node(node),
		_editor(editor),
		m_modelNode(modelNode)
      {          
      }

      virtual void onPositionChanged(const Dragger* sender, const osgEarth::GeoPoint& position)
      {
		  osgEarth::GeoPoint pos;
		  if (m_modelNode != NULL)
		  {
			  pos = position;
#if 0
			  //屏幕坐标转大地坐标
			  const osg::EllipsoidModel* ellip = _node->getMapNode()->getMapSRS()->getEllipsoid();
			  osg::Vec3d XYZpoint;
			  ellip->convertLatLongHeightToXYZ(osg::DegreesToRadians(pos.y()), osg::DegreesToRadians(pos.x()), pos.z(), XYZpoint.x(), XYZpoint.y(), XYZpoint.z());
			  //加包围盒半径
			  XYZpoint.z() = XYZpoint.z() + m_modelNode->getBound().radius();
			  //大地坐标转屏幕坐标
			  ellip->convertXYZToLatLongHeight(XYZpoint.x(), XYZpoint.y(), XYZpoint.z(), pos.y(), pos.x(), pos.z());
			  pos.y() = osg::RadiansToDegrees(pos.y());
			  pos.x() = osg::RadiansToDegrees(pos.x());
#endif
			  //pos.z() = pos.z() + m_modelNode->getBound().radius();
		  }
		  else
		  {
			  pos = position;
		  }

		  osg::Referenced* pro = const_cast<osg::Referenced*>(_node->getUserData());

		  if (pro) {

			  ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			  toolpro->setCenterPoint(pos.vec3d());
			  if (toolpro->getTextNode()) {
				  //qDebug() << "111111";
				  toolpro->getTextNode()->setPosition(position);
			  }
			  if (toolpro->getSignageNode()) {
				  toolpro->getSignageNode()->setPosition(position);
			  }
			  if (toolpro->getPosNode())
			  {
				  osg::Vec3d worldP;
				  position.toWorld(worldP);
				  toolpro->getPosNode()->setMatrix(osg::Matrixd::translate(worldP.x(), worldP.y(), worldP.z()));
			  }
			  if (toolpro->getToolType() == RECTANGLE)
			  {
				  std::vector<osg::Vec3d> points;
				  RectangleNode* rectnode = static_cast<RectangleNode*>(_node);
				  points.push_back(rectnode->getUpperLeft().vec3d());
				  points.push_back(rectnode->getLowerLeft().vec3d());
				  points.push_back(rectnode->getUpperRight().vec3d());
				  points.push_back(rectnode->getLowerRight().vec3d());

				  toolpro->setPoints(points);
			  }
		  }
          _node->setPosition(pos);
          _editor->updateDraggers();
      }

      GeoPositionNode* _node;
      GeoPositionNodeEditor* _editor;
	  osg::ref_ptr<osg::Node>m_modelNode;
};

/**********************************************************************/
AnnotationEditor::AnnotationEditor() :
osg::Group()
{
    // editor geometry should always be visible.
    osg::StateSet* stateSet = this->getOrCreateStateSet();
    stateSet->setMode(GL_DEPTH_TEST, 0);
    stateSet->setRenderBinDetails(99, "RenderBin");
}

/**********************************************************************/
GeoPositionNodeEditor::GeoPositionNodeEditor(GeoPositionNode* node, osg::ref_ptr<osg::Node>modelNode):
_node( node ),
m_modelNode(modelNode)
{
    _dragger  = new SphereDragger( _node->getMapNode());  
    _dragger->addPositionChangedCallback(new DraggerCallback(_node.get(), this,m_modelNode) );        
    addChild(_dragger);
    updateDraggers();
}

GeoPositionNodeEditor::~GeoPositionNodeEditor()
{    
}

void
GeoPositionNodeEditor::updateDraggers()
{
    GeoPoint pos = _node->getPosition();    
    _dragger->setPosition( pos, false );
}

void
GeoPositionNodeEditor::setPosition(const GeoPoint& pos)
{
    _node->setPosition( pos );
    updateDraggers();
}


/**********************************************************************/

class SetRadiusCallback : public Dragger::PositionChangedCallback
{
public:
    SetRadiusCallback(CircleNode* node, CircleNodeEditor* editor):
      _node(node),
      _editor( editor )
      {
      }

      virtual void onPositionChanged(const Dragger* sender, const osgEarth::GeoPoint& position)
      {
#if 1     
          //osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
          //而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
          const osg::EllipsoidModel* em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif
#if 0
          //改成如下方式使用，下面做相应修改 - Add by gkk 20220223
          const osgEarth::Ellipsoid em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif

          GeoPoint radiusLocation(position);
          radiusLocation.makeGeographic();

          //Figure out the distance between the center of the circle and this new location
          GeoPoint center = _node->getPosition();
          center.makeGeographic();

#if 1
          //Note by gkk 20220223
          double distance = GeoMath::distance(osg::DegreesToRadians( center.y() ), osg::DegreesToRadians( center.x() ), 
                                              osg::DegreesToRadians( radiusLocation.y() ), osg::DegreesToRadians( radiusLocation.x() ),
                                              em->getRadiusEquator());
#endif
#if 0
          //Add by gkk 20220223
		  double distance = GeoMath::distance(osg::DegreesToRadians(center.y()), osg::DegreesToRadians(center.x()),
                                              osg::DegreesToRadians(radiusLocation.y()), osg::DegreesToRadians(radiusLocation.x()),
                                              em.getRadiusEquator());
#endif 
		  osg::Referenced* pro = const_cast<osg::Referenced*>(_node->getUserData());

		  if (pro) {

			  ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			  toolpro->setCenterPoint(center.vec3d());
			  if (toolpro->getTextNode()) {
				  toolpro->getTextNode()->setPosition(position);
			  }
			  if (toolpro->getSignageNode()) {
				  toolpro->getSignageNode()->setPosition(position);
			  }
			  toolpro->setDisPoint(radiusLocation.vec3d());
		  }

          _node->setRadius( Linear(distance, Units::METERS ) );
          //The position of the radius dragger has changed, so recompute the bearing
          _editor->computeBearing();
      }

      CircleNode* _node;
      CircleNodeEditor* _editor;
};



CircleNodeEditor::CircleNodeEditor( CircleNode* node ):
GeoPositionNodeEditor( node ),
_radiusDragger( 0 ),
_bearing( osg::DegreesToRadians( 90.0 ) )
{
    _radiusDragger  = new SphereDragger(_node->getMapNode());
    _radiusDragger->addPositionChangedCallback(new SetRadiusCallback( node,this ) );        
    static_cast<SphereDragger*>(_radiusDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_radiusDragger);
    updateDraggers();
}

CircleNodeEditor::~CircleNodeEditor()
{
    //nop
}

void
CircleNodeEditor::setBearing( const Angle& bearing )
{
    _bearing = bearing.as(Units::RADIANS);
    updateDraggers();
}

void
CircleNodeEditor::computeBearing()
{
    _bearing = osg::DegreesToRadians( 90.0 );

    //Get the radius dragger's position
    if ( _radiusDragger->getPosition().isValid() )
    {
        // Get the current location of the center of the circle (in lat/long)
        GeoPoint location = _node->getPosition();
        location.makeGeographic();

        // location of the radius dragger (in lat/long)
        GeoPoint radiusLocation = _radiusDragger->getPosition();
        if ( !radiusLocation.getSRS()->isGeographic() )
            radiusLocation = radiusLocation.transform( location.getSRS() );

        // calculate the bearing b/w the 
        _bearing = GeoMath::bearing(
            osg::DegreesToRadians(location.y()), osg::DegreesToRadians(location.x()),
            osg::DegreesToRadians(radiusLocation.y()), osg::DegreesToRadians(radiusLocation.x()));
    }
}

void CircleNodeEditor::updateDraggers()
{
    GeoPositionNodeEditor::updateDraggers();
    if (_radiusDragger && _node->getMapNode())
    {
#if 1     
		//osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
		//而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
		const osg::EllipsoidModel* em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif
#if 0
		//改成如下方式使用，下面做相应修改 - Add by gkk 20220223
		const osgEarth::Ellipsoid em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif
        
        // Get the current location of the center of the circle (in lat/long, absolute Z)
        GeoPoint location = _node->getPosition();   
        location.makeGeographic();
        
        //Get the radius of the circle in meters
        double r = static_cast<CircleNode*>(_node.get())->getRadius().as(Units::METERS);

        double lat, lon;
#if 1
        //Note by gkk 20220223
        GeoMath::destination( osg::DegreesToRadians( location.y() ), 
                              osg::DegreesToRadians( location.x() ), 
                              _bearing, r, lat, lon, 
                              em->getRadiusEquator() );
#endif
#if 0
		//Add by gkk 20220223
		GeoMath::destination(osg::DegreesToRadians(location.y()),
                             osg::DegreesToRadians(location.x()),
			                 _bearing, r, lat, lon,
			                 em.getRadiusEquator());
#endif

        GeoPoint draggerLocation( 
            location.getSRS(),
            osg::RadiansToDegrees(lon),
            osg::RadiansToDegrees(lat));

        draggerLocation.z() = 0;

        _radiusDragger->setPosition( draggerLocation, false );
    }
}



/***************************************************************************************************/


class SetEllipseRadiusCallback : public Dragger::PositionChangedCallback
{
public:
    SetEllipseRadiusCallback(EllipseNode* node, EllipseNodeEditor* editor, bool major):
      _node(node),
      _editor( editor ),
      _major( major )
      {
      }

      virtual void onPositionChanged(const Dragger* sender, const osgEarth::GeoPoint& position)
      {
          if (!_node->getMapNode())
              return;     
#if 1     
		  //osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
		  //而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
          const osg::EllipsoidModel* em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif
#if 0
		  //改成如下方式使用，下面做相应修改 - Add by gkk 20220223
          const osgEarth::Ellipsoid em = _node->getMapNode()->getMapSRS()->getEllipsoid();
#endif

          //Figure out the distance between the center of the circle and this new location
          GeoPoint center = _node->getPosition();

#if 1
          //Note by gkk 20220223
          double distance = GeoMath::distance(osg::DegreesToRadians( center.y() ), osg::DegreesToRadians( center.x() ), 
                                              osg::DegreesToRadians( position.y() ), osg::DegreesToRadians( position.x() ),
                                              em->getRadiusEquator());
#endif
#if 0
          //Add by gkk 20220223
		  double distance = GeoMath::distance(
              osg::DegreesToRadians(center.y()), osg::DegreesToRadians(center.x()),
			  osg::DegreesToRadians(position.y()), osg::DegreesToRadians(position.x()),
			  em.getRadiusEquator());
#endif

          double bearing = GeoMath::bearing(osg::DegreesToRadians( center.y() ), osg::DegreesToRadians( center.x() ), 
                                            osg::DegreesToRadians( position.y() ), osg::DegreesToRadians( position.x() ));
        

          //Compute the new angular rotation based on how they moved the point
          if (_major)
          {
              _node->setRotationAngle( Angle( bearing-osg::PI_2, Units::RADIANS ) );
              _node->setRadiusMajor( Distance(distance, Units::METERS ) );
          }
          else // minor
          {
              _node->setRotationAngle( Angle( bearing, Units::RADIANS ) );
              _node->setRadiusMinor( Distance(distance, Units::METERS ) );
          }
          _editor->updateDraggers();
     }



      EllipseNode* _node;
      EllipseNodeEditor* _editor;
      bool _major;
};





EllipseNodeEditor::EllipseNodeEditor( EllipseNode* node ):
GeoPositionNodeEditor( node ),
_minorDragger( 0 ),
_majorDragger( 0 )
{
    _minorDragger  = new SphereDragger( _node->getMapNode());
    _minorDragger->addPositionChangedCallback(new SetEllipseRadiusCallback( node, this, false ) );        
    static_cast<SphereDragger*>(_minorDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_minorDragger);

    _majorDragger  = new SphereDragger( _node->getMapNode());
    _majorDragger->addPositionChangedCallback(new SetEllipseRadiusCallback( node, this, true ) );        
    static_cast<SphereDragger*>(_majorDragger)->setColor(osg::Vec4(1,0,0,1));
    addChild(_majorDragger);

    updateDraggers();
}

EllipseNodeEditor::~EllipseNodeEditor()
{
}

void EllipseNodeEditor::updateDraggers()
{
    GeoPositionNodeEditor::updateDraggers();
    if (_majorDragger && _minorDragger && _node->getMapNode())
    {
#if 1     
		//osgEarth3.2版本中 getEllipsoid()函数返回的是 const Ellipsoid& 类型，
		//而在osgEarth3.1版本中返回的是 const osg::EllipsoidModel* 类型 - Note by gkk 20220223
        const osg::EllipsoidModel* em = _node->getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid();
#endif
#if 0
		//改成如下方式使用，下面做相应修改 - Add by gkk 20220223
        const osgEarth::Ellipsoid em = _node->getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid();
#endif
        
        //Get the current location of the center of the circle
        GeoPoint location = _node->getPosition();    
        
        //Get the raddi of the ellipse in meters
        EllipseNode* ellipse = static_cast<EllipseNode*>(_node.get());
        double majorR = ellipse->getRadiusMajor().as(Units::METERS);
        double minorR = ellipse->getRadiusMinor().as(Units::METERS);

        double rotation = ellipse->getRotationAngle().as( Units::RADIANS );

        double latRad, lonRad;

        // minor dragger: end of the rotated +Y axis:
#if 1
        //Note by gkk 20220223
        GeoMath::destination(
            osg::DegreesToRadians( location.y() ), osg::DegreesToRadians( location.x() ), 
            rotation, 
            minorR, 
            latRad, lonRad, 
            em->getRadiusEquator());
#endif      

#if 0
        //Add by gkk 20220223
		GeoMath::destination(
			osg::DegreesToRadians(location.y()), osg::DegreesToRadians(location.x()),
			rotation,
			minorR,
			latRad, lonRad,
			em.getRadiusEquator());
#endif

        GeoPoint minorLocation(location.getSRS(), osg::RadiansToDegrees( lonRad ), osg::RadiansToDegrees( latRad ));
        minorLocation.z() = 0;       
        _minorDragger->setPosition( minorLocation, false );

        // major dragger: end of the rotated +X axis
#if 1
        //Note by gkk 20220223
        GeoMath::destination(
            osg::DegreesToRadians( location.y() ), 
            osg::DegreesToRadians( location.x() ), 
            rotation + osg::PI_2, 
            majorR, 
            latRad, lonRad, 
            em->getRadiusEquator()); 
#endif 

#if 0
        //Add by gkk 20220223
		GeoMath::destination(
			osg::DegreesToRadians(location.y()),
			osg::DegreesToRadians(location.x()),
			rotation + osg::PI_2,
			majorR,
			latRad, lonRad,
			em.getRadiusEquator());
#endif

        GeoPoint majorLocation(location.getSRS(), osg::RadiansToDegrees( lonRad ), osg::RadiansToDegrees( latRad ));
        majorLocation.z() = 0;
        _majorDragger->setPosition( majorLocation, false);
    }
}


/***************************************************************************************************/
class SetCornerDragger : public Dragger::PositionChangedCallback
{
public:
    SetCornerDragger(RectangleNode* node, RectangleNodeEditor* editor, RectangleNode::Corner corner):
      _node(node),
      _editor( editor ),
      _corner( corner )
      {
      }

      virtual void onPositionChanged(const Dragger* sender, const osgEarth::GeoPoint& position)
      {
          _node->setCorner( _corner, position ); 
		  _editor->updateDraggers();

		  osg::Referenced* pro = const_cast<osg::Referenced*>(_node->getUserData());

		  if (pro) {

			  ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			  std::vector<osg::Vec3d> points;

			  points.push_back(_node->getUpperLeft().vec3d());
			  points.push_back(_node->getLowerLeft().vec3d());
			  points.push_back(_node->getUpperRight().vec3d());
			  points.push_back(_node->getLowerRight().vec3d());

			  toolpro->setPoints(points);

			  osg::Vec3d lla, xyz = _node->getBound().center();
			  _node->getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(xyz.x(), xyz.y(), xyz.z(), lla.y(), lla.x(), lla.z());
			  lla.x() = osg::RadiansToDegrees(lla.x());
			  lla.y() = osg::RadiansToDegrees(lla.y());

			  toolpro->getTextNode()->setPosition(osgEarth::GeoPoint::GeoPoint(_node->getMapNode()->getMapSRS(), lla));

			  toolpro->getSignageNode()->setPosition(osgEarth::GeoPoint::GeoPoint(_node->getMapNode()->getMapSRS(), lla));
		  }
      }





	  

      RectangleNode* _node;
      RectangleNodeEditor* _editor;
      RectangleNode::Corner _corner;
};





RectangleNodeEditor::RectangleNodeEditor( RectangleNode* node ):
GeoPositionNodeEditor( node ),
_llDragger( 0 ),
_lrDragger( 0 ),
_urDragger( 0 ),
_ulDragger( 0 )
{
    //Lower left
    _llDragger  = new SphereDragger(_node->getMapNode());
    _llDragger->addPositionChangedCallback(new SetCornerDragger( node, this, RectangleNode::CORNER_LOWER_LEFT ) );        
    static_cast<SphereDragger*>(_llDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_llDragger);    

    //Lower right
    _lrDragger  = new SphereDragger(_node->getMapNode());
    _lrDragger->addPositionChangedCallback(new SetCornerDragger( node, this, RectangleNode::CORNER_LOWER_RIGHT ) );        
    static_cast<SphereDragger*>(_lrDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_lrDragger);    

    //Upper right
    _urDragger  = new SphereDragger(_node->getMapNode());
    _urDragger->addPositionChangedCallback(new SetCornerDragger( node, this, RectangleNode::CORNER_UPPER_RIGHT ) );        
    static_cast<SphereDragger*>(_urDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_urDragger);    

    //Upper left
    _ulDragger  = new SphereDragger(_node->getMapNode());
    _ulDragger->addPositionChangedCallback(new SetCornerDragger( node, this, RectangleNode::CORNER_UPPER_LEFT ) );        
    static_cast<SphereDragger*>(_ulDragger)->setColor(osg::Vec4(0,0,1,0));
    addChild(_ulDragger);    

    updateDraggers();
}

RectangleNodeEditor::~RectangleNodeEditor()
{
}

void
RectangleNodeEditor::updateDraggers()
{
    GeoPositionNodeEditor::updateDraggers();    

    RectangleNode* rect = static_cast<RectangleNode*>(_node.get());
    
    osg::Matrixd matrix;

    _ulDragger->setPosition( rect->getUpperLeft(), false);
    _llDragger->setPosition( rect->getLowerLeft(), false);
    _urDragger->setPosition( rect->getUpperRight(), false);    
    _lrDragger->setPosition( rect->getLowerRight(), false);
}
