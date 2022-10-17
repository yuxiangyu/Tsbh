#include "VisibilityTestArea.h"

#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <osg/TextureCubeMap>
#include <osg/BoundingSphere>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

#include <osgUtil/SmoothingVisitor>
#include <osgSim/OverlayNode>
#include <osgEarth/shadowing>
#include <osg/lightSource>
#include <osgEarth/registry>



static const int   SM_TEXTURE_WIDTH  = 1024;
static const bool  SHOW_DEBUG_CAMERA = false;

class SmoothingCallback: public osgDB::Registry::ReadFileCallback
{
public:
	virtual osgDB::ReaderWriter::ReadResult  readNode(const std::string &fileName, const osgDB::ReaderWriter::Options *options) override
	{
		osgDB::ReaderWriter::ReadResult  result     = osgDB::Registry::instance()->readNodeImplementation(fileName, options);
		osg::Node                       *loadedNode = result.getNode();

		if (loadedNode)
		{
			osgUtil::SmoothingVisitor  smoothing;
			loadedNode->accept(smoothing);
		}

		return result;
	}
};

static osg::ref_ptr<osg::Program>  generateShader(const QString &vertFile, const QString &fragFile, QString geomFile = "")
{
	osg::ref_ptr<osg::Program>  program = new osg::Program;

	if (!vertFile.isEmpty())
	{
		osg::ref_ptr<osg::Shader>  shader = new osg::Shader(osg::Shader::VERTEX);
		program->addShader(shader);
		shader=osgDB::readShaderFile(vertFile.toLocal8Bit().toStdString());
		
		if (!shader.valid())
		{
			osg::notify(osg::WARN) << "vertex program load failed" << std::endl;

			return nullptr;
		}
	}

	if (!fragFile.isEmpty())
	{
		osg::ref_ptr<osg::Shader>  shader = new osg::Shader(osg::Shader::FRAGMENT);
		program->addShader(shader);
		shader = osgDB::readShaderFile(vertFile.toLocal8Bit().toStdString());
		if (!shader.valid())
		{
			osg::notify(osg::WARN) << "fragment program load failed" << std::endl;

			return nullptr;
		}
	}

	if (!geomFile.isEmpty())
	{
		osg::ref_ptr<osg::Shader>  shader = new osg::Shader(osg::Shader::GEOMETRY);
		program->addShader(shader);
		shader = osgDB::readShaderFile(vertFile.toLocal8Bit().toStdString());
		if (!shader.valid())
		{
			osg::notify(osg::WARN) << "geometry program load failed" << std::endl;

			return nullptr;
		}
	}

	return program;
}

VisibilityTestArea::VisibilityTestArea(osg::Group* group,osg::View* view):
	_userRadius(200)
{
	
	//this->mapNode = node;
	this->_shadowedScene = new osg::Group;
	this->_parentScene = group;
	this->_parentScene->addChild(this->_shadowedScene);
	//直接在模型上生成
	_view = view;
}

void VisibilityTestArea::setParameter(const osg::Vec3d& Point1, double radius)
{
	_pickedPos2 = Point1;
	_userRadius = radius;
}

VisibilityTestArea::~VisibilityTestArea()
{
	_shadowedScene->removeChild(_currentDrawNode);
	_currentDrawNode = NULL;

	for (auto camera : _depthCameras)
	{
		_parentScene->removeChild(camera);
		camera = NULL;
	}

	_parentScene->getOrCreateStateSet()->setAttribute(_renderProgram, osg::StateAttribute::OFF);
	_parentScene->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::PROGRAM);
	_parentScene->getOrCreateStateSet()->removeTextureAttribute(1, osg::StateAttribute::TEXTURE);

	osgDB::Registry::instance()->setReadFileCallback(NULL);

	//_view->getCamera()->getGraphicsContext()->getState()->setUseModelViewAndProjectionUniforms(true);

	_shadowedScene = NULL;
}


osg::Geode* makeIndicator(osg::Vec3 eye)
{
	osg::ref_ptr<osg::Sphere>         pSphereShape   = new osg::Sphere(eye, 1.0f);
	osg::ref_ptr<osg::ShapeDrawable>  pShapeDrawable = new osg::ShapeDrawable(pSphereShape.get());

	pShapeDrawable->setColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	osg::ref_ptr<osg::Geode>  geode = new osg::Geode();
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	geode->addDrawable(pShapeDrawable.get());

	return geode.release();
}



osg::Camera * VisibilityTestArea::generateCubeCamera(osg::ref_ptr<osg::TextureCubeMap> cubeMap, unsigned face, osg::Camera::BufferComponent component)
{
	osg::ref_ptr<osg::Camera>  camera = new osg::Camera;

	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->setViewport(0, 0, SM_TEXTURE_WIDTH, SM_TEXTURE_WIDTH);
	camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	camera->attach(component, cubeMap, 0, face);

	camera->setNodeMask(0xffffffff & (~0x00000004));
	camera->addChild(_shadowedScene);
	//mRoot->addChild(camera);
	_parentScene->addChild(camera);
	return camera.release();
}







void  VisibilityTestArea::updateAttributes()
{
	// Light source info
	osg::Vec3  lightPos = _pickedPos2;
	

	_currentDrawNode = makeIndicator(lightPos);
	osgEarth::Registry::shaderGenerator().run(_currentDrawNode);
	_shadowedScene->addChild(_currentDrawNode);

	// Light source in shader
	float                     near_plane = 0.1f;
	float                     far_plane  = 500.0f;
	osg::Matrix               shadowProj = osg::Matrix::perspective(90.0, SM_TEXTURE_WIDTH / SM_TEXTURE_WIDTH, near_plane, far_plane);
	std::vector<osg::Matrix>  shadowViews;
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(1.0, 0.0, 0.0), osg::Vec3(0.0, -1.0, 0.0)));
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(-1.0, 0.0, 0.0), osg::Vec3(0.0, -1.0, 0.0)));
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(0.0, 1.0, 0.0), osg::Vec3(0.0, 0.0, 1.0)));
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(0.0, -1.0, 0.0), osg::Vec3(0.0, 0.0, -1.0)));
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(0.0, 0.0, 1.0), osg::Vec3(0.0, -1.0, 0.0)));
	shadowViews.push_back(
		osg::Matrix::lookAt(lightPos, lightPos + osg::Vec3(0.0, 0.0, -1.0), osg::Vec3(0.0, -1.0, 0.0)));

	// Update light source info for shadow map
	for (int i = 0; i < 6; i++)
	{
		auto  depthCamera = _depthCameras[i];
		depthCamera->setViewMatrix(shadowViews[i]);
		depthCamera->setProjectionMatrix(shadowProj);
		depthCamera->getOrCreateStateSet()->addUniform(new osg::Uniform("lightPos", lightPos));
		depthCamera->getOrCreateStateSet()->addUniform(new osg::Uniform("far_plane", far_plane));
		depthCamera->getOrCreateStateSet()->addUniform(new osg::Uniform("near_plane", near_plane));
		depthCamera->getOrCreateStateSet()->addUniform(new osg::Uniform("inverse_view", osg::Matrixf::inverse(shadowViews[i])));
	}

	// Update light source info for shadowing scene
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("lightPos", lightPos));
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("far_plane", far_plane));
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("near_plane", near_plane));
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("user_area", (float)_userRadius));
}







void VisibilityTestArea::buildModel()
{

	//_parentScene = _shadowedScene->getParent(0);
//	_view->getCamera()->getGraphicsContext()->getState()->setUseModelViewAndProjectionUniforms(true);

	osgUtil::SmoothingVisitor  smoothing;
	_shadowedScene->accept(smoothing);
	osgDB::Registry::instance()->setReadFileCallback(new SmoothingCallback);

	// Generate a shadow map
	osg::ref_ptr<osg::TextureCubeMap>  depthMap = new osg::TextureCubeMap;
	depthMap->setTextureSize(SM_TEXTURE_WIDTH, SM_TEXTURE_WIDTH);
	depthMap->setInternalFormat(GL_DEPTH_COMPONENT);
	depthMap->setSourceFormat(GL_DEPTH_COMPONENT);
	depthMap->setSourceType(GL_FLOAT);
	depthMap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	depthMap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	depthMap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
	depthMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	depthMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);

	// Depth shader that writes unnormaized depth into buffer
	osg::ref_ptr<osg::Program>  depthShader = generateShader(
		"resources/shaders/depthMap.vert", "resources/shaders/depthMap.frag");

	// Generate one camera for each side of the shadow cubemap
	for (int i = 0; i < 6; i++)
	{
		_depthCameras[i] = generateCubeCamera(depthMap, i, osg::Camera::DEPTH_BUFFER);
		_depthCameras[i]->getOrCreateStateSet()->setAttribute(depthShader, osg::StateAttribute::ON);
	}

	// Render the result in shader
	_renderProgram = generateShader("resources/shaders/visibilityShader.vert", "resources/shaders/visibilityShader.frag");

	if (!_renderProgram.valid())
	{
		//onRightButton();

		return;
	}

	_parentScene->getOrCreateStateSet()->setTextureAttributeAndModes(1, depthMap, osg::StateAttribute::ON);
	_parentScene->getOrCreateStateSet()->setAttribute(_renderProgram, osg::StateAttribute::ON);
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("baseTexture", 0));
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("shadowMap", 1));

	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("visibleColor", osg::Vec4(159 / 255, 1, 61 / 255, 1.0)));
	_parentScene->getOrCreateStateSet()->addUniform(new osg::Uniform("invisibleColor", osg::Vec4(1, 87 / 255, 61 / 255, 1.0)));

	updateAttributes();
	//_movingMode = true;

}
