#include "ToolManaMent.h"
#include<qdebug.h>
#include <osgViewer/View>
#include <osgEarth/Viewpoint>
#include <osgEarth/EarthManipulator>

using namespace osgEarth;

ToolManaMent::ToolManaMent(osgEarth::MapNode * mapNode, osg::Group * drawGroup, osgViewer::View* view)
	:m_MapNode(mapNode)
	, m_Root(drawGroup)
	, m_View(view)
{
	DrawLineTool* linetool= new DrawLineTool(m_MapNode, m_Root);
	DrawImageTool* imagetool = new DrawImageTool(m_MapNode, m_Root);
	DrawCircleTool* cricletool = new DrawCircleTool(m_MapNode, m_Root);
	DrawTextTool*  texttool = new DrawTextTool(m_MapNode, m_Root);
	DrawPolygonTool* polyontool = new DrawPolygonTool(m_MapNode, m_Root);
	DrawRectangleTool* rectangleytool = new DrawRectangleTool(m_MapNode, m_Root);
	DrawJBModeTool* jbmodeltool = new DrawJBModeTool(m_MapNode, m_Root);
	GeoGatheringPlace* geogathertool = new GeoGatheringPlace(m_MapNode, m_Root);
	DrawGeoPoint* geopostool = new DrawGeoPoint(m_MapNode, m_Root);

	m_View->addEventHandler(linetool);
	m_View->addEventHandler(imagetool);
	m_View->addEventHandler(cricletool);
	m_View->addEventHandler(texttool);
	m_View->addEventHandler(polyontool);
	m_View->addEventHandler(rectangleytool);
	m_View->addEventHandler(jbmodeltool);
	m_View->addEventHandler(geogathertool);
	m_View->addEventHandler(geopostool);

	m_Tools[LINE] = linetool;
	m_Tools[IMAGE] = imagetool;
	m_Tools[CIRCLE] = cricletool;
	m_Tools[TEXT] = texttool;
	m_Tools[POLYGON] = polyontool;
	m_Tools[RECTANGLE] = rectangleytool;
	m_Tools[MARK] = jbmodeltool;
	m_Tools[GEOGETHER] = geogathertool;
	m_Tools[GEOPOSITION] = geopostool;

	setAllActive(false);

}

ToolManaMent::~ToolManaMent()
{

}

void ToolManaMent::StartToolHandler(ToolType type)
{
	//M by gkk 20220810
	if (m_Tool)
	{
		if (m_Tool->getToolType() == type)
		{
			//如果当前绘制的标号类型(type)和上次绘制的标号类型(m_Tool->getToolType())一致,则直接返回
			return;
		} 
		else
		{
			//如果当前绘制的标号类型(type)和上次绘制的标号类型(m_Tool->getToolType())不一致,则说明此时又开始了其他类型标号的绘制
			//此时，需要判断上次的绘制是否是已经结束，如果未结束，则需要将上次绘制的标号删除掉，然后再开始新标号的绘制；如果已结束，则继续绘制新标号；
			if (m_Tool->getActive())
			{
				m_Tool->remove();
			}
		}
	}
	//===END===
	
	//if (m_Tool && m_Tool->getToolType() == type)
	//{
	//	return;
	//}

	setAllActive(false);

	switch (type)
	{
	case ToolType::LINE:
		m_Tool = m_Tools[LINE];
		break;
	case ToolType::IMAGE:
		m_Tool = m_Tools[IMAGE];
		break;
	case ToolType::CIRCLE:
		m_Tool = m_Tools[CIRCLE];
		break;
	case ToolType::TEXT:
		m_Tool = m_Tools[TEXT];
		break;
	case ToolType::POLYGON:
		m_Tool = m_Tools[POLYGON];
		break;
	case ToolType::RECTANGLE:
		m_Tool = m_Tools[RECTANGLE];
		break;
	case ToolType::MARK:
		m_Tool = m_Tools[MARK];
		break;
	case ToolType::GEOGETHER:
		m_Tool = m_Tools[GEOGETHER];
		break;
	case ToolType::GEOPOSITION:
		m_Tool = m_Tools[GEOPOSITION];
	default:
		break;
	}
	//m_Tool->setActive(true);
}

void ToolManaMent::RemoveToolHandler()
{
	if (m_Tool && m_View)
	{
		m_View->removeEventHandler(m_Tool);
	}
}

void ToolManaMent::setActive(bool isactive)
{
	if (m_Tool)
	{
		m_Tool->setActive(isactive);
	}
}

void ToolManaMent::ClearAllTool()
{
	if (m_Root->getNumChildren() <= 0)
	{
		return;
	}
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {
			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);
			if (toolpro)
			{
				if (toolpro->getTextNode())
				{
					m_Root->removeChild(toolpro->getTextNode());
				}
				if (toolpro->getPosNode())
				{
					m_Root->removeChild(toolpro->getPosNode());
				}
				if (toolpro->getDragNode())
				{
					m_Root->removeChild(toolpro->getDragNode());
				}
			}
			
			m_Root->removeChild(node);
		}
	}
	return;
}

void ToolManaMent::setAllActive(bool b)
{
	QMap<ToolType, osg::ref_ptr<DrawTool>>::iterator item = m_Tools.begin();
	while (item != m_Tools.end())
	{
		item.value()->setActive(b);
		item++;
	}
}

QList<osg::ref_ptr<osg::Node>> ToolManaMent::getToolNodes(ToolType type)
{
	QList<osg::ref_ptr<osg::Node>> listNodes;

	if (m_Root->getNumChildren() <= 0)
	{
		return listNodes;
	}
	
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		 osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		 osg::Referenced* pro = const_cast<osg::Referenced*>( node->getUserData());

		 if (pro) {

			 ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			 if (toolpro->getToolType() == type)
			 {
				 listNodes.append(node);
			 }
		 }
	}
	return listNodes;
}

QList<osg::ref_ptr<osg::Node>> ToolManaMent::getAllNodes()
{
	QList<osg::ref_ptr<osg::Node>> listNodes;

	if (m_Root->getNumChildren() <= 0)
	{
		return listNodes;
	}

	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		listNodes.append(node);
	}
	return listNodes;
}

void ToolManaMent::getToolNode(QString strId, osg::ref_ptr<osg::Node> &Node, osg::ref_ptr<osg::Node> &textNode)
{
	if (m_Root->getNumChildren() <= 0)
	{
		return ;
	}
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {

			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);
			if (toolpro->getNodeId() == strId)
			{
				Node = node;
				textNode = toolpro->getTextNode();
				break;
			}
		}
	}
}

osg::ref_ptr<osg::Node> ToolManaMent::getToolNode(unsigned int objectid)
{
	if (m_Root->getNumChildren() <= 0)
	{
		return NULL;
	}
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {

			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			if (toolpro->getNodeobjectId() == objectid)
			{
				return node;
			}
		}
	}
	return NULL;
}
void ToolManaMent::setNodeVisible(QString sID, bool bVisible)
{
	if (m_Root->getNumChildren() <= 0)
	{
		return ;
	}
	osg::ref_ptr<osg::Node> textNode;
	osg::ref_ptr<osg::Node> node;
	getToolNode(sID, node, textNode);
	if (node == nullptr)
		return;
	node->setNodeMask(bVisible);

	if (textNode != nullptr) {
		textNode->setNodeMask(bVisible);
	}

	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {

			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);
			if (toolpro->getNodeId() == sID)
			{
				if (toolpro->getPosNode())
				{
					toolpro->getPosNode()->setNodeMask(bVisible);
				}
				break;
			}
		}
	}
}
void ToolManaMent::locationNode(QString sID)
{
	if (m_Root->getNumChildren() <= 0)
	{
		return;
	}
	osg::ref_ptr<osg::Node> textNode;
	osg::ref_ptr<osg::Node> CurNode;
	getToolNode(sID, CurNode, textNode);
	if (CurNode == nullptr || textNode==nullptr)
		return;

	osgEarth::GeoPositionNode* pPositionNode = dynamic_cast<osgEarth::GeoPositionNode*>(CurNode.get());
	if (pPositionNode)
	{
		GeoPoint geoPt = pPositionNode->getPosition();
		//三维视角的飞跃
		double x = geoPt.x();
		double y = geoPt.y();
		double z = geoPt.z();
		if (z < 10)
		{
			//获取高程高度
			//m_MapNode->getTerrain()->getHeight(m_MapNode->getMapSRS(), x, y, &z);

			geoPt.z() = getElevation(x, y);
		}

		osgEarth::Viewpoint vp = osgEarth::Viewpoint("locationGeo", x, y, z, 0.0, -60.0, 500);
		
		osgEarth::EarthManipulator* mani=dynamic_cast<osgEarth::EarthManipulator*>(m_View->getCameraManipulator());
		mani->setViewpoint(vp);
		return;
	}
	osgEarth::FeatureNode* pFeatureNode = dynamic_cast<osgEarth::FeatureNode*>(CurNode.get());
	if (pFeatureNode)
	{
		Feature* pFea = pFeatureNode->getFeature();
		Geometry* geo = pFea->getGeometry();
		osg::Vec3Array* pvec2arr = geo->createVec3Array();
		if (pvec2arr && pvec2arr->size() > 0)
		{
			osg::Vec3d pt = pvec2arr->at(0);

			//三维视角的飞跃
			double x = pt.x();
			double y = pt.y();
			double z = pt.z();
			if (z < 10)
			{
				//获取高程高度
				m_MapNode->getTerrain()->getHeight(m_MapNode->getMapSRS(), x, y, &z);
			}

			//osgViewer:Viewpoint vp = osgViewer:Viewpoint("locationGeo", x, y, z, 0.0, -60.0, 500);
			//m_View->setViewpoint(vp);

			osgEarth::Viewpoint vp = osgEarth::Viewpoint("locationGeo", x, y, z, 0.0, -60.0, 500);

			osgEarth::EarthManipulator* mani = dynamic_cast<osgEarth::EarthManipulator*>(m_View->getCameraManipulator());
			mani->setViewpoint(vp);
		}

		return;
	}
}
void ToolManaMent::removeNode(QString sID)
{
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {

			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);
			if (toolpro->getNodeId() == sID)
			{
				if (toolpro->getTextNode())
				{
					m_Root->removeChild(toolpro->getTextNode());
				}
				if (toolpro->getPosNode())
				{
					m_Root->removeChild(toolpro->getPosNode());
				}
				if (toolpro->getDragNode())
				{
					m_Root->removeChild(toolpro->getDragNode());
				}

				m_Root->removeChild(node);			
				break;
			}
		}
	}
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticLine(std::vector<osg::Vec3d> points)
{

		DrawLineTool tool(m_MapNode, m_Root);
	   
		return tool.draw(points);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticCircle(osg::Vec3d cneterPoint, osg::Vec3d distancepoint)
{
	DrawCircleTool tool(m_MapNode, m_Root);

	return tool.draw(cneterPoint, distancepoint);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticGeoGatheringPlace(std::vector<osg::Vec2> points)
{
	GeoGatheringPlace tool(m_MapNode, m_Root);
	return tool.draw(points);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticRectangle(std::vector<osg::Vec3d> points)
{
	DrawRectangleTool tool(m_MapNode, m_Root);

	return tool.draw(points);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticPolygon(std::vector<osg::Vec3d> points)
{
	DrawPolygonTool tool(m_MapNode, m_Root);

	return tool.draw(points);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticWord(osg::Vec3d cneterPoint, QString sText)
{
	DrawTextTool tool(m_MapNode, m_Root);
	tool.setString(sText.toStdString());

	return tool.draw(cneterPoint);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticPos(osg::Vec3d cneterPoint)
{
	DrawGeoPoint tool(m_MapNode, m_Root);

	return tool.draw(cneterPoint);
}

osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticPic(osg::Vec3d cneterPoint, std::string path)
{
	DrawImageTool tool(m_MapNode, m_Root);

	tool.setImagePath(path);

	return tool.draw(cneterPoint);
}

/*************************************
 * Brief     : 标绘军标
 * Parameter : osg::Vec3d cneterPoint，中心点
 * Parameter : JBModelType type，军标类型
 * Parameter : E_PlottingRole role，军标角色，红\蓝 - add by gkk 20220516
 * Returns   : osg::ref_ptr<osg::Node>
*************************************/
osg::ref_ptr<osg::Node> ToolManaMent::DrawStaticJBModel(osg::Vec3d cneterPoint, JBModelType type, E_PlottingRole role)
{
	DrawJBModeTool tool(m_MapNode, m_Root);

	tool.setModelType(type);
	tool.setModelPath(m_sModelPath.toStdString());
	m_sModelPath = "";

	return tool.draw(cneterPoint, type, role);
}

osg::ref_ptr<osgEarth::PlaceNode> ToolManaMent::DrawStaticText(osg::ref_ptr<osg::Node> curNode,QString name)
{
	osgEarth::Style textStyle;

	textStyle.getOrCreate<TextSymbol>()->size() = 18;
	textStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	textStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	textStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	textStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	textStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

	AltitudeSymbol* alt = textStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;
	
	osg::Referenced* pro = const_cast<osg::Referenced*>(curNode->getUserData());
	if (pro) {

		ToolProperty* toolpro = static_cast<ToolProperty*>(pro);
		ToolType type = toolpro->getToolType();
		if (type == MARK)
		{
			osg::ref_ptr<osgEarth::PlaceNode> pLabelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), toolpro->getCenterPoint()), name.toStdString(), textStyle, NULL);
			pLabelNode->setDynamic(true);

			m_Root->addChild(pLabelNode);

			return pLabelNode;
		}
		else
		{
			osg::Vec3d xyz = curNode->getBound().center();
			osg::Vec3d lla;
			m_MapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(xyz.x(), xyz.y(), xyz.z(), lla.y(), lla.x(), lla.z());
			lla.x() = osg::RadiansToDegrees(lla.x());
			lla.y() = osg::RadiansToDegrees(lla.y());
			osg::ref_ptr<osgEarth::PlaceNode> pLabelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), lla), name.toStdString(), textStyle, NULL);
			pLabelNode->setDynamic(true);

			m_Root->addChild(pLabelNode);

			return pLabelNode;
		}
	}

	return NULL;

}

osg::ref_ptr<osgEarth::PlaceNode> ToolManaMent::DrawStaticText(osg::Vec3d point, QString name)
{
	osgEarth::Style textStyle;

	textStyle.getOrCreate<TextSymbol>()->size() = 18;
	textStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	textStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	textStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	textStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	textStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

	AltitudeSymbol* alt = textStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	osg::ref_ptr<osgEarth::PlaceNode> pLabelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), point), name.toStdString(), textStyle, NULL);
	pLabelNode->setDynamic(true);

	m_Root->addChild(pLabelNode);

	return pLabelNode;
}

osg::ref_ptr<SignageNode> ToolManaMent::DrawSignageText(osg::ref_ptr<osg::Node> curNode, QString text)
{
	osgEarth::Style m_labelStyle;
	m_labelStyle.getOrCreate<TextSymbol>()->size() = 18.0f;
	m_labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	m_labelStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	m_labelStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	m_labelStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	m_labelStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;
	m_labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(2, 100);

		SignageNode* mp_LabelNode = new SignageNode(m_MapNode,
			m_Root,
			osgEarth::GeoPoint(),
			text.toStdString(),
			m_labelStyle,
			SignageNode::Simple);

	mp_LabelNode->setDynamic(true);
	mp_LabelNode->setLabelType(1);
	mp_LabelNode->setStyle2(m_labelStyle);
	osgEarth::Registry::shaderGenerator().run(mp_LabelNode);

	osg::Vec3d xyz = curNode->getBound().center();
	osg::Vec3d lla;
	m_MapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(xyz.x(), xyz.y(), xyz.z(), lla.y(), lla.x(), lla.z());
	lla.x() = osg::RadiansToDegrees(lla.x());
	lla.y() = osg::RadiansToDegrees(lla.y());
	mp_LabelNode->setPosition(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), lla));
	m_Root->addChild(mp_LabelNode);
	return mp_LabelNode;
}

osg::ref_ptr<osgEarth::PlaceNode> ToolManaMent::DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec3d> points)
{
	osgEarth::Style textStyle;

	textStyle.getOrCreate<TextSymbol>()->size() = 18;
	textStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	textStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	textStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	textStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	textStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

	AltitudeSymbol* alt = textStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	double dmaxX, dmaxY, dmaxZ, dminX, dminY, dminZ;
	dmaxX = points.at(0).x();
	dminX = points.at(0).x();
	dmaxY = points.at(0).y();
	dminY = points.at(0).y();
	dmaxZ = points.at(0).z();
	dminZ = points.at(0).z();
	for (int i = 1; i < points.size(); i++)
	{
		if (dmaxX < points.at(i).x())
			dmaxX = points.at(i).x();
		if (dminX > points.at(i).x())
			dminX = points.at(i).x();
		if (dmaxY < points.at(i).y())
			dmaxY = points.at(i).y();
		if (dminY > points.at(i).y())
			dminY = points.at(i).y();
		if (dmaxZ < points.at(i).z())
			dmaxZ = points.at(i).z();
		if (dminZ > points.at(i).z())
			dminZ = points.at(i).z();
		
	}
	osg::Vec3d xyz;
	xyz.x() = (dmaxX + dminX) / 2;
	xyz.y() = (dmaxY + dminY) / 2;
	xyz.z() = (dmaxZ + dminZ) / 2;	

	osg::ref_ptr<osgEarth::PlaceNode> pLabelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), xyz), name.toStdString(), textStyle, NULL);
	pLabelNode->setDynamic(true);
	
	m_Root->addChild(pLabelNode);
	return pLabelNode;
}

osg::ref_ptr<osgEarth::PlaceNode> ToolManaMent::DrawStaticText(osg::ref_ptr<osg::Node> curNode, QString name, std::vector<osg::Vec2> points)
{
	osgEarth::Style textStyle;

	textStyle.getOrCreate<TextSymbol>()->size() = 18;
	textStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::White;
	textStyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	textStyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	textStyle.getOrCreate<TextSymbol>()->halo()->color() = Color(0.0, 0.0, 0.0, 0.8);
	textStyle.getOrCreate<TextSymbol>()->halo()->width() = 0.1;

	AltitudeSymbol* alt = textStyle.getOrCreate<AltitudeSymbol>();
	alt->verticalOffset() = 0.1;
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_DRAPE;

	double dmaxX, dmaxY, dminX, dminY;
	dmaxX = points.at(0).x();
	dminX = points.at(0).x();
	dmaxY = points.at(0).y();
	dminY = points.at(0).y();
	for (int i = 1; i < points.size(); i++)
	{
		if (dmaxX < points.at(i).x())
			dmaxX = points.at(i).x();
		if (dminX > points.at(i).x())
			dminX = points.at(i).x();
		if (dmaxY < points.at(i).y())
			dmaxY = points.at(i).y();
		if (dminY > points.at(i).y())
			dminY = points.at(i).y();
	}
	osg::Vec3d xyz;
	xyz.x() = (dmaxX + dminX) / 2;
	xyz.y() = (dmaxY + dminY) / 2;

	osg::ref_ptr<osgEarth::PlaceNode> pLabelNode = new osgEarth::PlaceNode(osgEarth::GeoPoint::GeoPoint(m_MapNode->getMapSRS(), xyz), name.toStdString(), textStyle, NULL);
	pLabelNode->setDynamic(true);

	m_Root->addChild(pLabelNode);
	return pLabelNode;
}

int ToolManaMent::getNodenum(ToolType type)
{
	int num = 0;
	if (m_Root->getNumChildren() <= 0)
	{
		return num;
	}
	for (int i = 0; i < m_Root->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::Node> node = m_Root->getChild(i);

		osg::Referenced* pro = const_cast<osg::Referenced*>(node->getUserData());

		if (pro) {

			ToolProperty* toolpro = static_cast<ToolProperty*>(pro);

			if (toolpro->getToolType() == type)
			{
				num++;
			}
		}
	}
	return num;
}

/*************************************
* Brief     : 获取地形高度
* Returns   : void
* Author    : ZZF 2022/06/26
*************************************/
double ToolManaMent::getElevation(double lon, double lat)
{
	if (!m_MapNode)
		return 0.0;
	// If getting the elevation fails, default to 0 to clamp to sea level
	double elevation = 0;

	osgEarth::GeoPoint point(m_MapNode->getMapSRS(), lon, lat, 0, osgEarth::ALTMODE_ABSOLUTE);
	osgEarth::ElevationSample sample = m_MapNode->getMap()->getElevationPool()->getSample(point, osgEarth::Distance(1.0, osgEarth::Units::METERS), nullptr);
	if (sample.hasData())
	{
		elevation = sample.elevation().as(osgEarth::Units::METERS);
	}
	else
	{
		double hamsl = 0.0; // not used
		double hae = 0.0; // height above ellipsoid, the rough elevation

		if (m_MapNode->getTerrain()->getHeight(m_MapNode->getMapSRS(), lon, lat, &hamsl, &hae))
			elevation = hae;
	}

	//return elevation;
	return elevation < 0.0 ? 0.0 : elevation;
}

