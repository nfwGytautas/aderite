#include "CameraProviderNode.hpp"

#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

CameraProviderNode::CameraProviderNode() {
    p_outputs.push_back(new OutputPin(
        this,
        node::getTypeName(node::PropertyType::Eye),
        "Eye"
    ));
}

const char* CameraProviderNode::getNodeName() const {
    return "Camera provider";
}

void CameraProviderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::CameraProvideOperation* op = new rendering::CameraProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

reflection::Type CameraProviderNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::CameraProviderNode);
}

bool CameraProviderNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool CameraProviderNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

