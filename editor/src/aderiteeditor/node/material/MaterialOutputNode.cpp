#include "MaterialOutputNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialOutputNode::MaterialOutputNode() 
{
    p_inputs.push_back(new InputPin(
		this,
        asset::prop::getNameForType(asset::prop::PropertyType::VEC4),
        "Color"
    ));
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

io::SerializableType MaterialOutputNode::getType() const {
    return static_cast<io::SerializableType>(io::EditorSerializables::MaterialOutputNode);
}

bool MaterialOutputNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool MaterialOutputNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

