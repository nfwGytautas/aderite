#include "MaterialOutputNode.hpp"

#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace node {

MaterialOutputNode::MaterialOutputNode() {
    p_inputs.push_back(new InputPin(this, asset::getNameForType(asset::PropertyType::VEC4), "Color"));
}

const char* MaterialOutputNode::getNodeName() const {
    return "Output";
}

void MaterialOutputNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    se->addFragmentColorInstruction(p_inputs[0]->getValue());
    m_evaluated = true;
}

reflection::Type MaterialOutputNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::MaterialOutputNode);
}

bool MaterialOutputNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool MaterialOutputNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

} // namespace node
} // namespace aderite
