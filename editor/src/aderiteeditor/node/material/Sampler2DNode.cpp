#include "Sampler2DNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"

#include "MaterialInputNode.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

namespace aderite {
namespace node {

Sampler2DNode::Sampler2DNode() {
    // Create pins
    p_inputs.push_back(new InputPin(this, asset::getNameForType(asset::SamplerType::TEXTURE_2D), "Texture"));

    p_outputs.push_back(new OutputPin(this, asset::getNameForType(asset::PropertyType::VEC4), "Color"));
}

const char* Sampler2DNode::getNodeName() const {
    return "Sampler 2D";
}

void Sampler2DNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    compiler::EvaluatorValue var = se->add2DSamplingInstruction(p_inputs[0]->getValue());
    p_outputs[0]->setValue(var);
    m_evaluated = true;
}

reflection::Type Sampler2DNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::Sampler2DNode);
}

bool Sampler2DNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool Sampler2DNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

} // namespace node
} // namespace aderite
