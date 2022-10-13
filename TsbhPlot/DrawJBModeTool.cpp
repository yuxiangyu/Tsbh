#include "DrawJBModeTool.h"
#include <qcoreapplication.h>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>
#include "transcoder.h"

DrawJBModeTool::DrawJBModeTool(osgEarth::MapNode * mapNode, osg::Group * drawGroup):
	DrawTool(mapNode,drawGroup)
	,m_mapNode(mapNode)
{
}

DrawJBModeTool::~DrawJBModeTool()
{
}

void DrawJBModeTool::beginDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;

}

void DrawJBModeTool::moveDraw(const osg::Vec3d & lla)
{
	if (lla == osg::Vec3d())
		return;

	_centerPoint = lla;

	if (m_ModelPalceNode == 0) 
	{
		initStyle();

		m_ModelPalceNode = new osgEarth::ModelNode(m_mapNode, m_modelStyle);

		osgEarth::Registry::objectIndex()->tagNode(m_ModelPalceNode, m_ModelPalceNode);

		_drawGroup->addChild(m_ModelPalceNode);
	}
#if 0
	//屏幕坐标转大地坐标
	const osg::EllipsoidModel* ellip = m_mapNode->getMapSRS()->getEllipsoid();
	osg::Vec3d point;
	ellip->convertLatLongHeightToXYZ(osg::DegreesToRadians(_centerPoint.y()), osg::DegreesToRadians(_centerPoint.x()),_centerPoint.z(), point.x(), point.y(), point.z());
	//加包围盒半径
	point.z() = point.z() + m_ModelNode->getBound().radius();
	//大地坐标转屏幕坐标
	ellip->convertXYZToLatLongHeight(point.x(), point.y(), point.z(), _centerPoint.y(), _centerPoint.x(), _centerPoint.z());
	_centerPoint.y() = osg::RadiansToDegrees(_centerPoint.y());
	_centerPoint.x() = osg::RadiansToDegrees(_centerPoint.x());
#endif

	m_ModelPalceNode->setPosition(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), _centerPoint));
}

void DrawJBModeTool::endDraw(const osg::Vec3d & lla)
{
	_temPoint.push_back(lla);

	if (m_ModelPalceNode) 
	{
		osg::Vec3d point = lla;

		m_geoeditr = new GeoPositionNodeEditor(m_ModelPalceNode,m_ModelNode);

		m_geoeditr->setNodeMask(false);

		m_Property = new ToolProperty;

		m_Property->setNodeStyle(m_modelStyle);

		m_Property->setJbType(m_type);

		m_Property->setJBRole(m_role);

		m_Property->setToolType(MARK);

		m_Property->setCenterPoint(point);

		m_Property->setDragNode(m_geoeditr);

		m_ModelPalceNode->setUserData(m_Property);

		m_ModelPalceNode->getUserData();

		_drawGroup->addChild(m_geoeditr);
	}
}

void DrawJBModeTool::resetDraw()
{
	_temPoint.clear();

	m_ModelPalceNode = nullptr;

	_centerPoint = osg::Vec3d();
}

osg::ref_ptr<osg::Node> DrawJBModeTool::draw(const osg::Vec3d & lla, JBModelType type, E_PlottingRole role)
{
	if(QString::fromStdString(m_modelPath).isEmpty())
		setModelPath(type, role);
	else
	{
		m_type = type;
		m_role = role;
	}
	initStyle();
	//构造提示label

	osg::Vec3d point = lla;
#if 0
	//屏幕坐标转大地坐标
	const osg::EllipsoidModel* ellip = m_mapNode->getMapSRS()->getEllipsoid();
	osg::Vec3d XYZpoint;
	ellip->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()), osg::DegreesToRadians(point.x()), point.z(), XYZpoint.x(), XYZpoint.y(), XYZpoint.z());
	//加包围盒半径
	XYZpoint.z() = XYZpoint.z() + m_ModelNode->getBound().radius();
	//大地坐标转屏幕坐标
	ellip->convertXYZToLatLongHeight(XYZpoint.x(), XYZpoint.y(), XYZpoint.z(), point.y(), point.x(), point.z());
	point.y() = osg::RadiansToDegrees(point.y());
	point.x() = osg::RadiansToDegrees(point.x());
#endif

	m_ModelPalceNode = new osgEarth::ModelNode(m_mapNode, m_modelStyle);

	m_ModelPalceNode->setPosition(osgEarth::GeoPoint::GeoPoint(m_mapNode->getMapSRS(), point));

	m_Property = new ToolProperty;

	m_Property->setNodeStyle(m_modelStyle);

	m_Property->setToolType(MARK);

	m_Property->setJbType(m_type);

	m_Property->setJBRole(m_role);	

	m_geoeditr = new GeoPositionNodeEditor(m_ModelPalceNode, m_ModelNode);

	m_geoeditr->setNodeMask(false);

	m_Property->setCenterPoint(point);

	m_Property->setDragNode(m_geoeditr);

	m_ModelPalceNode->setUserData(m_Property);

	osgEarth::Registry::objectIndex()->tagNode(m_ModelPalceNode, m_ModelPalceNode);

	_drawGroup->addChild(m_geoeditr);
	_drawGroup->addChild(m_ModelPalceNode);

	return m_ModelPalceNode;
}

void DrawJBModeTool::setStyle(osgEarth::Style style)
{
	m_modelStyle = style;
}

void DrawJBModeTool::setModelPath(JBModelType type, E_PlottingRole role)
{
	//add by gkk 20220810
	if (getActive())			
	{							//正在绘制中（即上一次的绘制过程还未结束）
		if (m_type != type)		//如果再次绘制的标号类型和正在绘制的标号类型不一致
		{
			remove();			//则应该先删除正在进行绘制的标号
		}
		else
		{
			//说明再次绘制的标号类型和正在绘制的标号类型一致，则判断绘制标号的角色是否一致
			if (m_role != role)
			{
				remove();	   //如果不一致，则应该先删除正在进行绘制的标号
			}
		}
	}
	//====END====

	m_type = type;
	m_role = role;

	std::string path = Transcoder::qstr2str(qApp->applicationDirPath());
	switch (type)
	{
	case ZXKMODEL:
		m_modelPath = path + "/resources/model/plottingModel/zxk.osg";
		break;
	case DJKMODEL:
		   m_modelPath = path + "/resources/model/plottingModel/djk.osg";
		break;
	case FSCDMODEL:
		   m_modelPath = path + "/resources/model/plottingModel/fszd.osg";
		break;
	case ZBYCMODEL:
		m_modelPath = path + "/resources/model/plottingModel/zbyk.osg";
		break;
	case DJQYMODEL:
		m_modelPath = path + "/resources/model/plottingModel/djqy.osg";
		break;
	case YQMODEL:
		m_modelPath = path + "/resources/model/plottingModel/yq.osg";
		break;
	case GFJWMODEL:
		m_modelPath = path + "/resources/model/plottingModel/gfjw.osg";
		break;
	case GLSKMODEL:
		m_modelPath = path + "/resources/model/plottingModel/glsk.osg";
		break;
	case TXGLMODEL:
		m_modelPath = path + "/resources/model/plottingModel/txgl.osg";
		break;
	case JB_HQBZBD:
		m_modelPath = path + "/resources/model/plottingModel/hqbzbd.osg";
		break;
	case JB_ZBBZBD:
		m_modelPath = path + "/resources/model/plottingModel/zbbzbd.osg";
		break;
	case JB_YLJHZ:
		m_modelPath = path + "/resources/model/plottingModel/yljhz.osg";
		break;
	case DXDDZXK:
		m_modelPath = path + "/resources/model/plottingModel/dxddzxk.osg";
		if (role == E_PlottingRole::BLUE)
			m_modelPath = path + "/resources/model/plottingModel/lan/dxddzxk.osg";
		
		break;
	case DXDDZXC:
		m_modelPath = path + "/resources/model/plottingModel/dxddzxc.osg";
		if (role == E_PlottingRole::BLUE)
			m_modelPath = path + "/resources/model/plottingModel/lan/dxddzxc.osg";
		
		break;
	case ZYZD:
		m_modelPath = path + "/resources/model/plottingModel/zyzd.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/zzzd.osg";
		break;
	case DSGDTXSN:
		m_modelPath = path + "/resources/model/plottingModel/dsgdtxsn.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/dsgdtxsn.osg";	
		break;
	case DDFSC:
		m_modelPath = path + "/resources/model/plottingModel/ddfsc.osg";
		if (role == E_PlottingRole::BLUE)
			m_modelPath = path + "/resources/model/plottingModel/lan/ddfsc.osg";
		break;
	case ZHC:
		m_modelPath = path + "/resources/model/plottingModel/zhc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/zhc.osg";
		break;
	case GCZBBZC:
		m_modelPath = path + "/resources/model/plottingModel/gczbbzc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/gczbbzc.osg";
		break;
	case ZCC:
		m_modelPath = path + "/resources/model/plottingModel/zcc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/zcc.osg";
		break;
	case TXC:
		m_modelPath = path + "/resources/model/plottingModel/txc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/txc.osg";
		break;
	case QC:
		m_modelPath = path + "/resources/model/plottingModel/qc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/qc.osg";
		break;
	case TZC:
		m_modelPath = path + "/resources/model/plottingModel/tzc.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/tzc.osg";
		break;
	case LJYCDKQBLD:
		m_modelPath = path + "/resources/model/plottingModel/ljycdkqbld.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/ljycdkqbld.osg";	
		break;
	case LDQBZ:
		m_modelPath = path + "/resources/model/plottingModel/ldqbz.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/ldqbz.osg";
		break;
	case FKBQZD:
		m_modelPath = path + "/resources/model/plottingModel/fkbqzd.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/fkbqzd.osg";
		break;
	case DDFSZD:
		m_modelPath = path + "/resources/model/plottingModel/ddfszd.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/ddfszd.osg";
		break;
	case YJJ:
		m_modelPath = path + "/resources/model/plottingModel/yjj.osg";
		if (role == E_PlottingRole::BLUE)
			m_modelPath = path + "/resources/model/plottingModel/lan/yjj.osg";
		break;
	case HZJ:
		m_modelPath = path + "/resources/model/plottingModel/hzj.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/hzj.osg";
		break;
	case ZDJ:
		m_modelPath = path + "/resources/model/plottingModel/zdj.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/zdj.osg";
		break;
	case HKMJ:
		m_modelPath = path + "/resources/model/plottingModel/hkmj.osg";
		if (role == E_PlottingRole::BLUE) 
			m_modelPath = path + "/resources/model/plottingModel/lan/hkmj.osg";	
		break;
	default:
		break;
	}
}

//读取模型文件路径，创建模型节点，设置模型样式
void DrawJBModeTool::initStyle()
{
	if (m_modelPath!= " ") 
	{
		m_ModelNode = osgDB::readNodeFile(m_modelPath);
		
		if (m_ModelNode == NULL)
			return;

		m_ModelNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		m_ModelNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		m_ModelNode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		m_ModelNode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
		m_ModelNode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		m_modelStyle.getOrCreate<ModelSymbol>()->scale() = 5;

		osg::MatrixTransform* _scaler = new osg::MatrixTransform;

		_scaler->setMatrix(osg::Matrixd::scale(1, 1, 1));
		_scaler->addChild(m_ModelNode);
		
		osg::AutoTransform* at = new osg::AutoTransform;
		at->setAutoScaleToScreen(true);
		at->setMaximumScale(50);
		at->addChild(_scaler);

		m_modelStyle.getOrCreate<ModelSymbol>()->setModel(at);

		//m_modelStyle.getOrCreate<ModelSymbol>()->pitch() = 90;
		m_modelStyle.getOrCreate<ModelSymbol>()->placement() = ModelSymbol::PLACEMENT_VERTEX;

		AltitudeSymbol* lalt = m_modelStyle.getOrCreate<AltitudeSymbol>();
		lalt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;				//贴地，线不是直线
																			//lalt->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;		//不显示
																			//lalt->clamping() = AltitudeSymbol::CLAMP_NONE;					//不贴地
																			//lalt->clamping() = AltitudeSymbol::CLAMP_ABSOLUTE;				//不贴地
																			//lalt->technique() = AltitudeSymbol::TECHNIQUE_GPU;				//运动时线会抖动
		lalt->technique() = AltitudeSymbol::TECHNIQUE_MAP;					//运动时线不会抖动
																			//lalt->technique() = AltitudeSymbol::TECHNIQUE_SCENE;				//运动时线不会抖动
																			//lalt->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;				//画出的线会变粗
		lalt->verticalOffset() = 0.1;
	}
}

std::vector<osg::Vec3d> DrawJBModeTool::getPoints()
{
	return _temPoint;
}

void DrawJBModeTool::setModelPath(std::string path)
{
	m_modelPath = path;
}

void DrawJBModeTool::setModelType(JBModelType type)
{
	m_type = type;
}

NodeInfo DrawJBModeTool::getNodeinfos()
{
	NodeInfo info;
	if (m_ModelPalceNode)
	{
		info.node = m_ModelPalceNode;
		info.anneditr = m_geoeditr;
	}
	return info;
}

void DrawJBModeTool::remove()
{
	if (m_ModelPalceNode)
	{
		_drawGroup->removeChild(m_ModelPalceNode);
		m_ModelPalceNode = nullptr;
		_temPoint.clear();
	}
}
