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
#include "aderiteeditor/runtime/OperationArray.hpp"

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

void ConvertNode::setFromType(const std::string& from) {
	p_inputs[0]->setType(from);

	if (pipeline::isArray(from) && !pipeline::isArray(m_to)) {
		// Convert output to array
		p_outputs[0]->setType(m_to + "[]");
	}
	else if (!pipeline::isArray(from) && pipeline::isArray(m_to)) {
		// Remove array
		p_outputs[0]->setType(m_to.substr(0, m_to.length() - 2));
	}

	// TODO: remove m_to and m_from
	m_to = p_outputs[0]->getType();
}

void ConvertNode::setToType(const std::string& to) {
	p_outputs[0]->setType(to);

	if (pipeline::isArray(to) && !pipeline::isArray(m_from)) {
		// Convert output to array
		p_inputs[0]->setType(m_from + "[]");
	}
	else if (!pipeline::isArray(to) && pipeline::isArray(m_from)) {
		// Remove array
		p_inputs[0]->setType(m_from.substr(0, m_from.length() - 2));
	}

	// TODO: remove m_to and m_from
	m_from = p_inputs[0]->getType();
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

bool ConvertNode::onConnectToInput(InputPin* target, OutputPin* source) {
	if (source->getType() != m_from) {
		this->setFromType(source->getType());
	}

	return true;
}

bool ConvertNode::onConnectToOutput(OutputPin* target, InputPin* source) {
	if (source->getType() != m_to) {
		this->setToType(source->getType());
	}

	return true;
}

void ConvertNode::handlePipelineConvert(compiler::PipelineEvaluator* pe) {
	if (pipeline::isArrayOrType(m_from, pipeline::getTypeName(pipeline::PropertyType::Camera))) {
		if (pipeline::isArrayOrType(m_to, pipeline::getTypeName(pipeline::PropertyType::Eye))) {
			convert(pe, &ConvertNode::eyeToCamera);
			return;
		}
	}

	LOG_ERROR("Unknown conversion from {0} to {1}", m_from, m_to);
}

void ConvertNode::handleShaderConvert(compiler::ShaderEvaluator* se) {

}

void ConvertNode::convert(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn) {
	if (pipeline::isArray(m_from)) {
		// Array convert
		applyArray(pe, cfn);
	}
	else {
		rendering::OperationBase* from = pe->getOperation(p_inputs[0]->getValue());
		rendering::OperationBase* to = (this->*cfn)(from);
		p_outputs[0]->setValue(pe->addOperation(to));
	}
}

void ConvertNode::applyArray(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn) {
	runtime::OperationArray* oparr = static_cast<runtime::OperationArray*>(pe->getOperation(p_inputs[0]->getValue()));
	runtime::OperationArray* result = new runtime::OperationArray();
	for (rendering::OperationBase* op : *oparr) {
		rendering::OperationBase* converted = (this->*cfn)(op);
		pe->addOperation(converted);
		result->addOperation(converted);
	}
	result->setDebugName("Convert (" + m_from + "->" + m_to + ")");
	p_outputs[0]->setValue(pe->addOperation(result));
}

rendering::OperationBase* ConvertNode::eyeToCamera(rendering::OperationBase* from) {
	rendering::EyeProvideOperation* op = new rendering::EyeProvideOperation();
	op->provideFromCamera(
		static_cast<rendering::CameraProvideOperation*>(from)
	);
	return op;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
