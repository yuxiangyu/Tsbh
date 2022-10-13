#include "PlotPickerCallback.h"
#include <osgEarth/Feature>
#include <osgEarth/FeatureIndex>
#include <osgEarth/PlaceNode>
#include <qdebug.h>
#include <simVis/Shaders.h>

const char* highlightVert = R"(
    #version 330
    uniform uint objectid_to_highlight;
    uniform float osg_FrameTime;
    uint oe_index_objectid;      // Stage global containing object id
    flat out int selected;
    flat out float Time;
    void checkForHighlight(inout vec4 vertex)
    {
        selected = (objectid_to_highlight > 1u && objectid_to_highlight == oe_index_objectid) ? 1 : 0;
        Time = osg_FrameTime;
    }
)";
const char* highlightFrag = R"(
    #version 330
    flat in int selected;
    flat in float Time;
    void highlightFragment(inout vec4 color)
    {
        if ( selected == 1 ){
               color.rgb = mix(color.rgb, clamp(vec3(0.5,2.0,2.0)*(1.0-color.rgb), 0.0, 1.0), 0.5);
         }
    }
)";

PlotPickerCallback::PlotPickerCallback(osg::ref_ptr<osgEarth::MapNode>mapNode):
	m_mapNode(mapNode)
{
	installHighlighter();
}


PlotPickerCallback::~PlotPickerCallback()
{
}

void PlotPickerCallback::onHit(ObjectID id)
{
	FeatureIndex* index = Registry::objectIndex()->get<FeatureIndex>(id).get();
	Feature* feature = index ? index->getFeature(id) : 0L;
	//选中feature的操作
	if (feature)
	{
		//_app.fidLabel->setText(Stringify() << "Feature ID = " << feature->getFID() << " (oid = " << id << ")");
		//_app.nameLabel->setText(Stringify() << "Name = " << feature->getString("name"));
	}

	else
	{
		// Check whether it's an annotation:
		//PlaceNode* anno = Registry::objectIndex()->get<PlaceNode>(id).get();
		osg::Node*  anno  = Registry::objectIndex()->get<osg::Node>(id).get();
		
		if (anno)
		{
			
			//_app.fidLabel->setText(Stringify() << "ObjectID = " << id);
			//_app.nameLabel->setName(Stringify() << "Name = " << anno->getName());
			std::string nn=anno->getName();
			osg::Referenced* pro = const_cast<osg::Referenced*>(anno->getUserData());

			if (pro) {
				if (m_toolpro)
				{
					if (m_toolpro->getDragNode()) {
						m_toolpro->getDragNode()->setNodeMask(false);
					}
					if (m_toolpro->getSignageNode())
					{
						m_toolpro->getSignageNode()->setNodeMask(false);
					}
				}
				m_toolpro = static_cast<ToolProperty*>(pro);
				if (m_type == 0) {
					if (m_toolpro->getDragNode()) {
						m_toolpro->getDragNode()->setNodeMask(true);
					}
				}
				else if (m_type == 1)
				{
					if (m_toolpro->getSignageNode()) {
						std::string text = m_toolpro->getSignageNode()->getText();
						if (text == "") {
							m_toolpro->getSignageNode()->setText(m_toolpro->getToolName().toStdString());
						}
						m_toolpro->getSignageNode()->setNodeMask(true);
					}
				}
				m_toolpro->setNodeobjectId(id);

		/*		if (toolpro->getNodeobjectId() == id)
				{
					qDebug() << "whl---------------";
				}*/
			}
			//emit this->signalCameraActor(QString::fromStdString(nn));
		}

		// None of the above.. clear.
		else
		{
			//_app.fidLabel->setText(Stringify() << "unknown oid = " << id);
			//_app.nameLabel->setText(" ");
			
		}
	}

	//_app.highlightUniform->set(id);
	//highlightUniform->set(id);
	if (m_type == 0) {
		emit signalCameraActor(id);
	}
}

void PlotPickerCallback::onMiss()
{
	if (m_toolpro)
	{
		if (m_type == 0) {
			if(m_toolpro->getDragNode())
			{
				m_toolpro->getDragNode()->setNodeMask(false);
			}		
		}
		else if (m_type == 1)
		{
			if (m_toolpro->getSignageNode()) {
				m_toolpro->getSignageNode()->setNodeMask(false);
			}
		}
	}
	//highlightUniform->set(0u);
	if (m_type == 0) {
		emit signalCameraActor(0);
	}
}

bool PlotPickerCallback::accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa)
{

	return ea.getEventType() == ea.PUSH && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
}

void PlotPickerCallback::installHighlighter()
{
	osg::StateSet* stateSet = m_mapNode->getOrCreateStateSet();
	int attrLocation = Registry::objectIndex()->getObjectIDAttribLocation();

	// This shader program will highlight the selected object.
	VirtualProgram* vp = VirtualProgram::getOrCreate(stateSet);


	vp->setFunction("checkForHighlight", highlightVert, ShaderComp::LOCATION_VERTEX_CLIP);
	vp->setFunction("highlightFragment", highlightFrag, ShaderComp::LOCATION_FRAGMENT_COLORING);

	// Since we're accessing object IDs, we need to load the indexing shader as well:
	Registry::objectIndex()->loadShaders(vp);

	// A uniform that will tell the shader which object to highlight:
	highlightUniform = new osg::Uniform("objectid_to_highlight", 0u);
	TimeUniform = new osg::Uniform("osg_FrameTime", 3.0);
	stateSet->addUniform(highlightUniform);
	stateSet->addUniform(TimeUniform);
}
