#include "SameCenter.h"
#include "osgEarth/ExtrusionSymbol"
#include "osgEarth/Skins"
#include <osgEarth/GLUtils>
#include <osg/BlendFunc>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/ObjectIndex>
#include "transcoder.h"

using namespace osgEarth;
using namespace osgEarth::Util;

SameCenter::SameCenter(osgEarth::MapNode* mapNode, osg::Group* drawGroup)
{
	this->_mapNode = mapNode;
	this->_drawGroup = drawGroup;
	_maxHeight=0;
	group = new osg::Group;

}

SameCenter::SameCenter(osgEarth::MapNode* mapNode, osg::Group* drawGroup,double maxHeight)
{
	this->_mapNode = mapNode;
	this->_drawGroup = drawGroup;
	_maxHeight = maxHeight;
	group = new osg::Group;
}

void SameCenter::draw(int CircleNum,double MaxRadius,osg::Vec3d center,int status, std::string name)
{
	const char* highlightFrag = R"(
    in vec3 v_pos;
void apply_haze(inout vec4 color)
{
    float dist = clamp( length(v_pos)/10000000.0, 0, 0.75 );
    color = mix(color, vec4(0.5, 0.5, 0.5, 1.0), dist);
//color =mix(color.rgb, clamp(vec3(0.5,2.0,2.0)*(1.0-color.rgb), 0.0, 1.0), 0.5);
}
)";

	const char* haze_vertex = R"(
	out vec3 v_pos;
	void setup_haze(inout vec4 vertexView)
	{
		v_pos = vertexView.xyz;
	}
)";
	this->m_iCircleNum = CircleNum;
	m_dMaxRadius = MaxRadius;
	m_center = center;
	this->status = status;
	this->name = name;
	 dd = (int)(m_dMaxRadius / (m_iCircleNum*2));
	//osgEarth::Style style;
	
	style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping()
		= osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::AltitudeSymbol>()->technique()
		= osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->widthUnits()
		= osgEarth::Units::METERS;
	//�ֱ������ã�ֵԽ��Խ��ϸ��Բ��Բ��
	style.getOrCreate<osgEarth::LineSymbol>()->tessellation() = 50;
	//style.getOrCreate<osgEarth::RenderSymbol>()->lighting() = false;
	std::string string1=name;
	//����ɫ����
	switch (status)
	{
	case 1:
		color1 = osgEarth::Color(81 / 255.0, 191 / 255.0, 1, 0.8);
		string1 += "-δ��ʼ";
		break;
	case 2:
		color1 = osgEarth::Color(177 / 255.0, 1, 1, 0.8);
		string1 += "-������";
		break;
	case 3:
		color1 = osgEarth::Color(200 / 255.0, 200 / 255.0, 200 / 255.0, 0.8);
		string1 += "-ִ����";
		break;
	case 4:
		color1 = osgEarth::Color(105 / 255.0, 229 / 255.0, 140 / 255.0, 0.8);
		string1 += "-�����";
		break;
	case 5:
		color1 = osgEarth::Color(1, 133 / 255.0, 59 / 255.0, 0.8);
		string1 += "-�ѹ���";
		break;
	default:
		break;
	}
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = color1;
	
	//�߿������
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = dd;


		//������������
		osg::ref_ptr<osg::Vec3dArray> pCircleVertex = new osg::Vec3dArray;

	//kΪ��ǰ�����Ƶ�Ȧ�㣬�������⻭��m_iCircleNumΪ�ⲿ������Ϊͬ��Բ��Ȧ��
	for (int k = 1; k <= m_iCircleNum; k++)
	{
		//ÿ��ѭ������һ�����飬��Ϊ��������������Ĵ��ڣ�Ҫ��ʱ��������һȦ�����������pCircleVertex���л���
		pCircleVertex->clear();
		//��ǰ������Ȧ����ԭ��İ뾶radiusΪ���뾶-��ͬ��Բ����-��ǰ���ƵĲ�����*�����뾶/һ����Ҫ���ƵĲ������������뾶-��δ���Ʋ���*ÿ��İ뾶���õ��ѻ��Ƶİ뾶
		double radius = m_dMaxRadius - (double)(m_iCircleNum - k) * (m_dMaxRadius / (double)m_iCircleNum);
		osg::ref_ptr<osgEarth::Geometry> plinenode
			= new osgEarth::LineString();
		osg::ref_ptr<osgEarth::Feature> Feature
			= new osgEarth::Feature(plinenode, _mapNode->getMapSRS()->getGeographicSRS());
		//_featureNode = new FeatureNode(
		//	_feature, _lineStyle);
		//360/5Ϊ72����
		for (int i = 0; i <= 360; i = i + 5)
		{
			//����
			double angle = (double)i * osg::PI / 180.0;
			double x, y;
			//destination����Ϊ���������Ļ����µ�ά����Rad y��Rad x���Ƕ�angle�Ͱ뾶radius�������Ŀ���Ļ����µ�γ�Ⱥ;���Rad y��Rad x
			//GeoMath�����кܶ�����ļ��㷽������distance֮��ģ�����ȡ��
			osgEarth::GeoMath::destination(m_center.y()* osg::PI / 180.0, m_center.x()* osg::PI / 180.0, angle, radius, y, x);
			//����ת����
			y = y * 180.0 / osg::PI;
			x = x * 180.0 / osg::PI;
			//���㴫������
			//pCircleVertex->push_back(osg::Vec3d(x, y, m_center.z()));
			Feature->getGeometry()->push_back(osg::Vec3d(x, y, m_center.z()));
		}
		//����������볡��


		osg::ref_ptr<osgEarth::FeatureNode> FeatureNode
			= new osgEarth::FeatureNode(Feature, style);
		FeatureNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, 0);
		osgEarth::GLUtils::setLighting(FeatureNode->getOrCreateStateSet(), 1);
		osgEarth::GLUtils::setLineSmooth(FeatureNode->getOrCreateStateSet(),1);
		VirtualProgram* vp = VirtualProgram::getOrCreate(FeatureNode->getOrCreateStateSet());
		vp->setFunction("setup_haze", haze_vertex, ShaderComp::LOCATION_VERTEX_VIEW);
		vp->setFunction("apply_haze", highlightFrag, ShaderComp::LOCATION_FRAGMENT_LIGHTING);
			
		// Since we're accessing object IDs, we need to load the indexing shader as well:
		osgEarth::Registry::objectIndex()->loadShaders(vp);
		FeatureNode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ZERO), 1);
		group->addChild(FeatureNode);
		FeatureNode->setMapNode(_mapNode);

		m_vecFeatureNode.push_back(FeatureNode);
	}

	//Style textstyle;

	//style.getOrCreate<BBoxSymbol>()->geom() = BBoxSymbol::BboxGeom::GEOM_BOX_ORIENTED;
	textstyle.getOrCreate<BBoxSymbol>()->border()->width() = 2.0;
	textstyle.getOrCreate<BBoxSymbol>()->border()->color() = Color(1.0, 1.0, 1.0, 1.0);
	textstyle.getOrCreate<TextSymbol>()->size() = 20.0f;
	textstyle.getOrCreate<TextSymbol>()->fill()->color() = Color(1.0, 1.0, 1.0, 1.0);
	textstyle.getOrCreate<TextSymbol>()->font() = "c:/windows/fonts/simhei.ttf";
	textstyle.getOrCreate<TextSymbol>()->encoding() = osgEarth::TextSymbol::ENCODING_UTF8;
	textstyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_BOTTOM_BASE_LINE;
	std::string ss;
	Transcoder::gb2312ToUtf8(string1, ss);
	labelNode = new LabelNode(ss, textstyle);
	labelNode->setPosition(GeoPoint(_mapNode->getMapSRS()->getGeographicSRS(), m_center.x(), m_center.y(), m_center.z()));
	labelNode->setDynamic(true);
	//osgEarth::Registry::shaderGenerator().run(labelNode);
	//����Բ��
	group->addChild(labelNode);
	renderCircleNode();
	if (_maxHeight > 0)
	{
		osg::LOD* lod = (osg::LOD*)_drawGroup;
		lod->addChild(group, 0, _maxHeight);
	}
	else
		_drawGroup->addChild(group);
}

void SameCenter::remove()
{
	if(group.valid())
	{ 
		_drawGroup->removeChild(group);
		
	}
}

void SameCenter::renderCircleNode()
{
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0, 0, 0), dd/2);
	osg::Geode* geode = new osg::Geode();
	_shapeDrawable = new osg::ShapeDrawable(shape);
	_shapeDrawable->setDataVariance(osg::Object::DYNAMIC);
	_shapeDrawable->setColor(color1);
	geode->addDrawable(_shapeDrawable);

	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

	osg::Material *material = new osg::Material;
	material->setDiffuse(osg::Material::FRONT, osg::Vec4(0.486, 0.486, 0.811, 1.0));
	material->setAmbient(osg::Material::FRONT, osg::Vec4(0.486, 0.486, 0.811, 1.0));
	//mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 0.0, 0.0, 1.0));
	//�������澵����ɫ
	material->setSpecular(osg::Material::FRONT, osg::Vec4(1.0, 1.0, 1.0, 1.0));
	//��������ָ��
	//mat->setShininess(osg::Material::FRONT, 90.0);
	material->setShininess(osg::Material::FRONT, 90.0);
	material->setColorMode(osg::Material::AMBIENT);
	geode->getOrCreateStateSet()->setAttribute(material);
		
	//osgUtil::SmoothingVisitor::smooth(*(_shapeDrawable.get()));//�Զ����ɷ���
	osg::Vec3d worldP;
	osgEarth::GeoPoint point(_mapNode->getMapSRS()->getGeographicSRS(), m_center.x(), m_center.y(), m_center.z());
	point.toWorld(worldP);

	//mapPoint.createLocalToWorld(matrix);

	osg::MatrixTransform* _scaler = new osg::MatrixTransform;
	_scaler->addChild(geode);
	_scaler->setMatrix(osg::Matrixd::translate(worldP.x(), worldP.y(), worldP.z()));
		

	group->addChild(_scaler);
}
	


void SameCenter::setStatus(int status)
{
	std::string string1 = name;
	//����ɫ����
	switch (status)
	{
	case 1:
		color1 = osgEarth::Color(81 / 255.0, 191 / 255.0, 1, 0.5);
		string1 += "-δ��ʼ";
		break;
	case 2:
		color1 = osgEarth::Color(177 / 255.0, 1, 1, 0.8);
		string1 += "-������";
		break;
	case 3:
		color1 = osgEarth::Color(200 / 255.0, 200 / 255.0, 200 / 255.0, 0.5);
		string1 += "-ִ����";
		break;
	case 4:
		color1 = osgEarth::Color(105 / 255.0, 229 / 255.0, 140 / 255.0, 0.5);
		string1 += "-�����";
		break;
	case 5:
		color1 = osgEarth::Color(1, 133 / 255.0, 59 / 255.0, 0.5);
		string1 += "-�ѹ���";
		break;
	default:
		break;
	}
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = color1;

	for each (auto var in m_vecFeatureNode)
	{
		var->setStyle(style);	
		var->dirty();
	}

	std::string ss;
	Transcoder::gb2312ToUtf8(string1, ss);
	labelNode->setText(ss);
	labelNode->setStyle(textstyle);

	_shapeDrawable->setColor(color1);
}
