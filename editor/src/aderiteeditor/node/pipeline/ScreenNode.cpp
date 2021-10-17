#include "ScreenNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/OutputToScreenOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

ScreenNode::ScreenNode()
{
    p_inputs.push_back(new InputPin(
        this,
        node::getTypeName(node::PropertyType::Target),
        "Target"
    ));

    p_inputs.push_back(new InputPin(
        this,
        node::getTypeName(node::PropertyType::Require),
        "Require"
    ));

    // Make require optional
    p_inputs[1]->setOptional(true);
}

const char* ScreenNode::getNodeName() const {
    return "Screen";
}

void ScreenNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::OutputToScreenOperation* op = new rendering::OutputToScreenOperation(
        static_cast<rendering::TargetProvideOperation*>(pe->getOperation(p_inputs[0]->getValue()))
    );
    pe->addOperation(op);
    m_evaluated = true;
}

io::SerializableType ScreenNode::getType() {
    return static_cast<io::SerializableType>(io::EditorSerializables::ScreenNode);
}

bool ScreenNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool ScreenNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

