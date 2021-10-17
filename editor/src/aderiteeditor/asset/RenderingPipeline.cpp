#include "RenderingPipeline.hpp"

#include "aderite/io/Serializer.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"

namespace aderite {
namespace asset {

RenderingPipeline::RenderingPipeline() 
	: m_graph(new node::Graph())
{
	node::ScreenNode* sn = m_graph->addNode<node::ScreenNode>();
	m_graph->setLastNode(sn);
}

RenderingPipeline::~RenderingPipeline() {
	delete m_graph;
}

node::Graph* RenderingPipeline::getGraph() {
	return m_graph;
}

io::SerializableType RenderingPipeline::getType() {
	return static_cast<io::SerializableType>(io::EditorSerializables::RenderingPipelineAsset);
}

bool RenderingPipeline::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Graph";
	serializer->writeUntrackedType(emitter, m_graph);
	return true;
}

bool RenderingPipeline::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	delete m_graph;
	m_graph = static_cast<node::Graph*>(serializer->parseUntrackedType(data["Graph"]));
	return true;
}

}
}
