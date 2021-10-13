#include "ConvertNode.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"

#include "aderite/rendering/operation/All.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

ConvertNode::ConvertNode(int id, Graph* graph, const std::string& from, const std::string& to)
	: Node(id), m_from(from), m_to(to)
{
	p_inputs.push_back(graph->createInputPin(
		this,
		m_from,
		"From"
	));

	p_outputs.push_back(graph->createOutputPin(
		this,
		m_to,
		"To"
	));
}

const char* ConvertNode::getNodeName() const {
	return "Convert";
}

void ConvertNode::evaluate(compiler::GraphEvaluator* evaluator) {
	evaluateDependencies(evaluator);

	compiler::PipelineEvaluator* pe = dynamic_cast<compiler::PipelineEvaluator*>(evaluator);
	if (pe != nullptr) {
		handlePipelineConvert(pe);
	}
	else {
		handleShaderConvert(dynamic_cast<compiler::ShaderEvaluator*>(evaluator));
	}

	m_evaluated = true;
}

bool ConvertNode::serialize(YAML::Emitter& out) {
	out << YAML::BeginMap;
	out << YAML::Key << "NodeType" << YAML::Value << "Convert";
	out << YAML::Key << "From" << YAML::Value << m_from;
	out << YAML::Key << "To" << YAML::Value << m_to;
	serializeData(out);
	out << YAML::EndMap;
	return false;
}

bool ConvertNode::deserialize(YAML::Node& data) {
	deserializeData(data);
	return true;
}

void ConvertNode::handlePipelineConvert(compiler::PipelineEvaluator* pe) {
	if (m_from == pipeline::getTypeName(pipeline::PropertyType::Camera)) {
		if (m_to == pipeline::getTypeName(pipeline::PropertyType::Eye)) {
			rendering::EyeProvideOperation* op = new rendering::EyeProvideOperation();
			op->provideFromCamera(
				static_cast<rendering::CameraProvideOperation*>(pe->getOperation(p_inputs[0]->getValue()))
			);
			p_outputs[0]->setValue(pe->addOperation(op));
			return;
		}
	}

	LOG_ERROR("Unknown conversion from {0} to {1}", m_from, m_to);
}

void ConvertNode::handleShaderConvert(compiler::ShaderEvaluator* se) {

}

ADERITE_EDITOR_NODE_NAMESPACE_END
