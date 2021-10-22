#include "RenderingPipeline.hpp"

#include "aderiteeditor/windows/backend/node/imnodes.h"

#include "aderite/utility/Log.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

namespace aderite {
namespace asset {

RenderingPipeline::RenderingPipeline() 
	: m_graph(new node::Graph())
{
	node::ScreenNode* sn = m_graph->addNode<node::ScreenNode>();
	m_graph->setLastNode(sn);
	ImNodes::SetNodeGridSpacePos(sn->getId(), ImVec2(200, 200));
}

RenderingPipeline::~RenderingPipeline() {
	delete m_graph;
}

node::Graph* RenderingPipeline::getGraph() const {
	return m_graph;
}

void RenderingPipeline::compile() {
	LOG_TRACE("Compiling rendering pipeline");
	compiler::PipelineEvaluator evaluator;
	
	m_graph->resetEvaluateFlag();
	m_graph->getLastNode()->evaluate(&evaluator);
	evaluator.transferToPipeline(this);
}

bool RenderingPipeline::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	Pipeline::serialize(serializer, emitter);

	emitter << YAML::Key << "Graph";
	serializer->writeUntrackedType(emitter, m_graph);
	return true;
}

bool RenderingPipeline::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	Pipeline::deserialize(serializer, data);

	delete m_graph;
	m_graph = static_cast<node::Graph*>(serializer->parseUntrackedType(data["Graph"]));
	return true;
}

}
}
