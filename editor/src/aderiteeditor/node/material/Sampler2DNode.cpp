#include "Sampler2DNode.hpp"

#include "MaterialInputNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Sampler2DNode::Sampler2DNode()
{
    // Create pins
    p_inputs.push_back(
        new InputPin(
            this,
            asset::prop::getNameForType(
                asset::prop::PropertyType::TEXTURE_2D), 
                "Texture"
        )
    );

    p_outputs.push_back(
        new OutputPin(
            this,
            asset::prop::getNameForType(
                asset::prop::PropertyType::VEC4), 
            "Color"
        )
    );
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

io::SerializableType Sampler2DNode::getType() const {
    return static_cast<io::SerializableType>(io::EditorSerializables::Sampler2DNode);
}

bool Sampler2DNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool Sampler2DNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

