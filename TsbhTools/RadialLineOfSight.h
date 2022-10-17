
#ifndef UTIL_LINEOFSIGHT
#define UTIL_LINEOFSIGHT

#include <osgEarth/LineOfSight>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>
#include <osgEarth/Draggers>


    using namespace osgEarth;
	using namespace osgEarth::Contrib;

	struct PointVisResult
	{
		GeoPoint p;
		int lineId;
		bool bVis;

		PointVisResult(const GeoPoint& point, int id, bool b)
		{
			p = point;
			lineId = id;
			bVis = b;
		}
		PointVisResult()
		{
			
		}
	};
    /**
     * A Node that can be used to display radial line of sight calculations
     */
    class  RadialLineOfSightNode2 : public LineOfSightNode, public MapNodeObserver
    {
    public:
        /**
         * Create a new RadialLineOfSightNode
         * @param mapNode
         *        The MapNode this RadialLineOfSightNode is operating on.
         */
        RadialLineOfSightNode2( MapNode* mapNode );

        virtual ~RadialLineOfSightNode2();

        /**
         * Sets the radius in meters
         */
        void setRadius( double radius );
        
        /**
         * Gets the radius in meters
         */
        double getRadius() const;

        /**
         * Sets the number of spokes in the radial line of sight calculation
         */
        void setNumSpokes( int numSpokes );

        /**
         * Gets the number of spokes in the radial line of sight calculation
         */
        int getNumSpokes() const;

        /**
         * Gets the center point in world coordinates
         */
        const osg::Vec3d& getCenterWorld() const;

        /**
         * Gets the center point
         */
        const GeoPoint& getCenter() const;

        /**
         * Set the center point.  The point should be in the Map's coordinate system.  So if you're dealing with a geocentric map
         * the location should be in the form lon, lat, elevation
         */
        void setCenter(const GeoPoint& center);

        /**
         * Sets the good color
         */
        void setGoodColor( const osg::Vec4f &color );

        /**
         * Gets the good color
         */
        const osg::Vec4f& getGoodColor() const;


        /**
         * Sets the bad color
         */
        void setBadColor( const osg::Vec4f &color );

        /**
         * Gets the bad color
         */
        const osg::Vec4f& getBadColor() const;

        /**
         * Sets the outline color
         */
        void setOutlineColor( const osg::Vec4f &color );

        /**
         * Gets the outline color
         */
        const osg::Vec4f& getOutlineColor() const;

        /**
         * Gets the display mode
         */
        LineOfSight::DisplayMode getDisplayMode() const;

        /*
         * Sets the display mode
         */
        void setDisplayMode( LineOfSight::DisplayMode displayMode );

        /**
         * Gets whether to draw the fill of this RadialLineOfSightNode
         */
     //   bool getFill() const;

        /*
         * Sets whether to draw the fill of this RadialLineOfSightNode
         */
       // void setFill( bool fill );


        void addChangedCallback( LOSChangedCallback* callback );
        
        void removeChangedCallback( LOSChangedCallback* callback );


        /**
         * Called when the underlying terrain has changed.
         */
        void terrainChanged( const osgEarth::TileKey& tileKey, osg::Node* terrain );
        

        bool getTerrainOnly() const;
        void setTerrainOnly( bool terrainOnly );


    public: // MapNodeObserver

        virtual void setMapNode( MapNode* mapNode );

        MapNode* getMapNode() { return _mapNode.get(); }


    private:
        osg::Node* getNode();
        void compute(osg::Node* node);
     //   void compute_line(osg::Node* node);
        void compute_fill(osg::Node* node);
        int _numSpokes;
        double _radius;

        LineOfSight::DisplayMode _displayMode;    
     //   bool _fill;
        osg::Vec4 _goodColor;
        osg::Vec4 _badColor;
        osg::Vec4 _outlineColor;
        GeoPoint   _center;        
        osg::Vec3d _centerWorld;
        osg::observer_ptr< MapNode > _mapNode;        
        LOSChangedCallbackList _changedCallbacks;        
        osg::ref_ptr < osgEarth::TerrainCallback > _terrainChangedCallback;
        bool _terrainOnly;
		std::vector<PointVisResult> insideResults;
		std::vector<PointVisResult> outerResults;

		//��ȡ�ڻ���������
		void renderInsideGeometry( int index);
		//��ȡ�⻷�Ķ����
		void renderOuterGeometry(int index);

		void renderGeometry();
    };

    /**********************************************************************/

     /**
     * An update callback that allows you to attach a RadialLineOfSightNode to a moving node.
     * The update callback will update the center point of the calcuation to follow the node.
     *
     * Example:
     * RadialLineOfSightNode* los = new RadialLineOfSightNode(myMapNode);
     * los->setUpdateCallback( new RadialLineOfSightTether( myNode ) );
     */
    class RadialLineOfSightTether2 : public osg::NodeCallback
    {
    public:
        RadialLineOfSightTether2(osg::Node* node);
        virtual ~RadialLineOfSightTether2() { }
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);  

        osg::Node* node() { return _node.get(); }

    private:
        osg::ref_ptr< osg::Node > _node;
    };


    /**********************************************************************/

    class RadialLineOfSightEditor2 : public osg::Group
    {
    public:
        /**
         * Create a new RadialLineOfSightEditor
         * @param los
         *        The RadialLineOfSightNode to edit
         */
        RadialLineOfSightEditor2(RadialLineOfSightNode2* los);    
        virtual ~RadialLineOfSightEditor2();

         /**
         *Updates the position of the dragger to represent the actual location of the RadialLineOfSightNode.
         *This should be called if the los is changed outside of the editor and would probably benefit
         *from the RadialLineOfSightNode having a callback that notifies listeners that the start/end points have changed.
         */
        void updateDraggers();
    private:
        osg::ref_ptr< RadialLineOfSightNode2 > _los;
        osgEarth::Dragger* _dragger;
        osg::ref_ptr< LOSChangedCallback > _callback;
    };





#endif // OSGEARTHUTIL_LINEOFSIGHT
