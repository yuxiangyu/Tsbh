//////////////////////////////////////////////////////////////////////////
/// Copyright (C), 1998-2016, rytec Corporation. All rights reserved.
/// \file       RichLabelNode.h
/// \brief      ��ʾ��ά��ע��Ϣ�ڵ�
/// \author     LiXingQiang
/// \version    0.95
/// \date       2019-3
//////////////////////////////////////////////////////////////////////////

#ifndef _RYESCENE_RICHLABELNODE_H_
#define _RYESCENE_RICHLABELNODE_H_

//#include "Render/EScene/Export.h"

#include <osgEarth/GeoPositionNode>
#include <osgEarth/BboxDrawable>
#include <osgEarth/Style>
#include <osgEarth/MapNode>
#include <osgEarth/ScreenSpaceLayout>
#include <osg/Geode>

using namespace osgEarth;



/// \brief ��ע��Ϣ�ڵ�  ESCENE_EXPORT
class  RichLabelNode :public osgEarth::GeoPositionNode
{
public:
	RichLabelNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
		const Style& style = Style());

	/// \brief Constructs a label node that is positioned relative to a map.
	RichLabelNode(MapNode* mapNode, const GeoPoint& position, const std::string& text,
		const TextSymbol* symbol);

	/// \brief Deserializes a label
	RichLabelNode(MapNode* mapNode, const Config& conf, const osgDB::Options* dbOptions);

	RichLabelNode(osg::Group* Group, const GeoPoint& position, const std::string& text,
		const Style& style = Style());

	RichLabelNode* buildLabel(const GeoPoint& pos, const std::string& text, osg::Group* root, MapNode* mapNode);

	virtual ~RichLabelNode() {}

public:
	/// \brief ������ʾ�ı���Ϣ
	/// \param [in] text ��ʾ�ı���Ϣ
	void setText(const std::string& text);

	/// \brief ��ȡ��ʾ�ı���Ϣ
	/// \return ��ʾ�ı���Ϣ
	const std::string& getText() const { return mText; }

	/// \brief ���ñ�ע��ʾ��ʽ
	/// \param [in] style ��ע��ʾ��ʽ
	void setStyle(const Style& style);

	/// \brief ��ȡ��ע��ʾ��ʽ
	/// \return ��ע��ʾ��ʽ
	const Style& getStyle() const { return mStyle; }

public: // GeoPositionNode override
		/// \brief Priority of the item.A negative Priority means "never occlude me".
		/// \param [in] value ���ȼ�
	virtual void setPriority(float value);

	/// \brief Sets the node to "dynamic",
	/// i.e. sets up the node so that you can safely change it at runtime.
	/// \param [in] value �ڵ��Ƿ��Ƕ�̬��
	virtual void setDynamic(bool value);

	virtual Config getConfig() const;

	/// \brief called when someone calls one of the set functions
	virtual void dirty();

protected:
	/** Copy constructor */
	RichLabelNode(const RichLabelNode& rhs, const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);

	void init(const Style& style);

	void updateLayoutData();

protected:
	std::string              mText;
	Style                    mStyle;
	osg::ref_ptr<osg::Geode> mGeode;
	osg::ref_ptr<ScreenSpaceLayoutData> mDataLayout;

	/** rotation of the label **/
	float                    mLabelRotationRad;
	bool                     mFollowFixedCourse;
	GeoPoint                 mGeoPointLoc;
	GeoPoint                 mGeoPointProj;
	osg::ref_ptr<osgText::Text> mTextGeometry;
	osg::ref_ptr<osgEarth::BboxDrawable> mBBoxGeom;
};


#endif // _RYESCENE_RICHLABELNODE_H_
