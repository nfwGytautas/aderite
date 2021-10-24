#include "EntitiesNode.hpp"

#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EntitiesNode::EntitiesNode()
{
    p_outputs.push_back(new OutputPin(
        this,
        std::string(node::getTypeName(node::PropertyType::Entity)) + "[]",
        "Entities"
    ));
}

const char* EntitiesNode::getNodeName() const {
    return "Entities";
}

void EntitiesNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::EntityProvideOperation* op = new rendering::EntityProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

reflection::Type EntitiesNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EntitiesNode);
}

bool EntitiesNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool EntitiesNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

