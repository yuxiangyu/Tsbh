#pragma once
#include <osgEarth/GeoPositionNode>
#include <osgEarth/BboxDrawable>
#include <osgEarth/Style>
#include <osgEarth/MapNode>
#include <osgEarth/ScreenSpaceLayout>
#include <osg/Geode>
#include "TsbhPlot_global.h"

using namespace osgEarth;

/// \brief 标注信息节点  ESCENE_EXPORT
class  TsbhPlot_EXPORT SignageNode :public osgEarth::GeoPositionNode
{

public:
	enum RichType {
		Simple,
		Complex1,
		Complex2

	};

	SignageNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
		const Style& style = Style(), const RichType&rt = Simple);

	SignageNode(MapNode* mapNode,osg::Group *group, const GeoPoint& position, const std::string& text,
		const Style& style = Style(), const RichType&rt = Simple);

	/// \brief Constructs a label node that is positioned relative to a map.
	SignageNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
		const TextSymbol* symbol, const RichType&rt = Simple);

	/// \brief Deserializes a label
	SignageNode(MapNode* mapNode, const Config& conf, const osgDB::Options* dbOptions, const RichType&rt = Simple);

	SignageNode* buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode);

	virtual ~SignageNode() {}

public:
	/// \brief 设置显示文本信息
	/// \param [in] text 显示文本信息
	void setText(const std::string& text);
	void setVisiable(bool v);

	void setLabelType(int type);
	/// \brief 获取显示文本信息
	/// \return 显示文本信息
	const std::string& getText(){ return mText; }

	/// \brief 设置标注显示样式
	/// \param [in] style 标注显示样式
	void setStyle2(const Style& style);

	/// \brief 获取标注显示样式
	/// \return 标注显示样式
	const Style& getStyle() const { return mStyle; }

	void updatePosition(double x, double y, double z);

	void remove();

public: // GeoPositionNode override
		/// \brief Priority of the item.A negative Priority means "never occlude me".
		/// \param [in] value 优先级
	virtual void setPriority(float value);
	std::string& getID();
	void  setID(std::string id) {
		mID = id;
	}
	/// \brief Sets the node to "dynamic",
	/// i.e. sets up the node so that you can safely change it at runtime.
	/// \param [in] value 节点是否是动态的
	virtual void setDynamic(bool value);

	virtual Config getConfig() const;

	void init(const Style& style);

	/// \brief called when someone calls one of the set functions
	virtual void dirty();

protected:
	/** Copy constructor */
	SignageNode(const SignageNode& rhs, const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);


	void updateLayoutData();

protected:
	std::string              mText;
	std::string              mID;

	Style                    mStyle;
	osg::ref_ptr<osg::Geode> mGeode;
	osg::ref_ptr < osgEarth::MapNode> m_mapnode;
	osg::ref_ptr<ScreenSpaceLayoutData> mDataLayout;

	/** rotation of the label **/
	float                    mLabelRotationRad;
	bool                     mFollowFixedCourse;
	RichType                 richtype;
	GeoPoint                 mGeoPointLoc;
	GeoPoint                 mGeoPointProj;
	osg::ref_ptr<osgText::Text> mTextGeometry;
	osg::ref_ptr<osg::Geometry> mBBoxGeom;
	const GeoPoint m_position;
	const Style m_style;
	int m_type = 0;
};



