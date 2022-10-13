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
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef OSGEARTHANNO_ANNOTATION_EDITING_H
#define OSGEARTHANNO_ANNOTATION_EDITING_H 1

#include "TsbhPlot_global.h"

#include <osgEarth/Common>
#include <osgEarth/GeoPositionNode>
#include <osgEarth/CircleNode>
#include <osgEarth/EllipseNode>
#include <osgEarth/RectangleNode>
#include <osgEarth/Draggers>
#include <osg/Group>


using namespace osgEarth;
    /**
     * @deprecated (remove or refactor as example after 2.10)
     */
    class TsbhPlot_EXPORT AnnotationEditor : public osg::Group
    {
    protected:
        AnnotationEditor();

        virtual ~AnnotationEditor() { }
    };

     /**
     * An editor node that allows you to move the position of LocalizedNode annotations 
     * @deprecated (remove or refactor as example after 2.10)   
     */
    class TsbhPlot_EXPORT GeoPositionNodeEditor : public AnnotationEditor
    {
    public:
        /**
         * Create a new LocalizedAnnotationEditor
         * @param localizedNode
         *        The LocalizedNode to edit
         */
        GeoPositionNodeEditor(osgEarth::GeoPositionNode* localizedNode,osg::ref_ptr<osg::Node>modelNode = NULL);    
        
        virtual ~GeoPositionNodeEditor();    

        virtual void updateDraggers();

		osgEarth::Dragger* getPositionDragger() { return _dragger; }

        void setPosition( const osgEarth::GeoPoint& pos );

    protected:
        osg::ref_ptr< osgEarth::GeoPositionNode > _node;
		osgEarth::Dragger* _dragger;
		osg::ref_ptr<osg::Node> m_modelNode;
    };

    /**
     * @deprecated (remove or refactor as example after 2.10)   
     */
    class TsbhPlot_EXPORT CircleNodeEditor : public GeoPositionNodeEditor
    {
    public:
        /**
         * Create a new CircleEditor
         * @param circleNode
         *        The CircleNode to edit
         */
        CircleNodeEditor(CircleNode* circleNode);    
        
        virtual ~CircleNodeEditor();    

        void computeBearing();

        void setBearing( const Angle& value );

        virtual void updateDraggers();

        Dragger* getRadiusDragger() { return _radiusDragger; }

        double _bearing;

        Dragger* _radiusDragger;
    };
    
    /**
     * @deprecated (remove or refactor as example after 2.10)   
     */
    class TsbhPlot_EXPORT EllipseNodeEditor : public GeoPositionNodeEditor
    {
    public:
        /**
         * Create a new EllipseNodeEditor
         * @param ellipseNode
         *        The EllipseNodeEditor to edit
         */
        EllipseNodeEditor(EllipseNode* ellipseNode);    
        
        virtual ~EllipseNodeEditor();    

        virtual void updateDraggers();

        Dragger* _majorDragger;
        Dragger* _minorDragger;
    };
    
    /**
     * @deprecated (remove or refactor as example after 2.10)   
     */
    class TsbhPlot_EXPORT RectangleNodeEditor : public GeoPositionNodeEditor
    {
    public:
        /**
         * Create a new RectangleNodeEditor
         * @param rectangleNode
         *        The RectangleNodeEditor to edit
         */
        RectangleNodeEditor(RectangleNode* rectangleNode);    
        
        virtual ~RectangleNodeEditor();    

        virtual void updateDraggers();

        Dragger* _llDragger;
        Dragger* _lrDragger;
        Dragger* _urDragger;
        Dragger* _ulDragger;
    };


#endif // OSGEARTHANNO_FEATURE_EDITING_H
