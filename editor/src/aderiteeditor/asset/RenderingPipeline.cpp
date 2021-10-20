#include "RenderingPipeline.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

namespace aderite {
namespace asset {

RenderingPipeline::RenderingPipeline() 
	: m_graph(new node::Graph()), m_pipeline(new rendering::Pipeline())
{
	node::ScreenNode* sn = m_graph->addNode<node::ScreenNode>();
	m_graph->setLastNode(sn);
}

RenderingPipeline::~RenderingPipeline() {
	delete m_graph;
}

node::Graph* RenderingPipeline::getGraph() const {
	return m_graph;
}

rendering::Pipeline* RenderingPipeline::getPipeline() const {
	return m_pipeline;
}

void RenderingPipeline::compile() {
	LOG_TRACE("Compiling rendering pipeline");
	compiler::PipelineEvaluator evaluator;

	m_graph->resetEvaluateFlag();
	m_graph->getLastNode()->evaluate(&evaluator);

	m_pipeline = evaluator.constructPipeline();
}

io::SerializableType RenderingPipeline::getType() const {
	return static_cast<io::SerializableType>(io::EditorSerializables::RenderingPipelineAsset);
}

bool RenderingPipeline::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Graph";
	serializer->writeUntrackedType(emitter, m_graph);
	emitter << YAML::Key << "Pipeline";
	serializer->writeType(emitter, m_pipeline);
	return true;
}

bool RenderingPipeline::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	delete m_graph;
	m_graph = static_cast<node::Graph*>(serializer->parseUntrackedType(data["Graph"]));

	delete m_pipeline;
	m_pipeline = static_cast<rendering::Pipeline*>(serializer->parseType(data["Pipeline"]));

	return true;
}

}
}
