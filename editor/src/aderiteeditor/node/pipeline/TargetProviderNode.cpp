#include "TargetProviderNode.hpp"

#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

TargetProviderNode::TargetProviderNode()
{
    p_outputs.push_back(new OutputPin(
        this,
        node::getTypeName(node::PropertyType::Target),
        "Target"
    ));
}

const char* TargetProviderNode::getNodeName() const {
    return "Target provider";
}

void TargetProviderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::TargetProvideOperation* op = new rendering::TargetProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

reflection::Type TargetProviderNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::TargetProviderNode);
}

bool TargetProviderNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool TargetProviderNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

