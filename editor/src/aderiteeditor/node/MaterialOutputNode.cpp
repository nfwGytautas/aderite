#include "MaterialOutputNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderWriter.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialOutputNode::MaterialOutputNode(int id, Graph* graph) 
    : Node(id)
{
    p_inputs.push_back(graph->createInputPin(
		this,
        asset::prop::getNameForType(asset::prop::PropertyType::VEC4),
        "Color"
    ));
}

const char* MaterialOutputNode::getNodeName() const {
    return "Output";
}

void MaterialOutputNode::evaluate(compiler::ShaderWriter* writer) {
    evaluateDependencies(writer);
    writer->addFragmentColorInstruction(p_inputs[0]->getValue());
    m_evaluated = true;
}

bool MaterialOutputNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "MaterialOutput";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool MaterialOutputNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

