#include "CameraProviderNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

CameraProviderNode::CameraProviderNode(int id, Graph* graph)
    : Node(id)
{
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Camera),
        "Camera"
    ));
}

const char* CameraProviderNode::getNodeName() const {
    return "Camera provider";
}

void CameraProviderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::MainCameraProvideOperation* op = new rendering::MainCameraProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool CameraProviderNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Camera";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool CameraProviderNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

