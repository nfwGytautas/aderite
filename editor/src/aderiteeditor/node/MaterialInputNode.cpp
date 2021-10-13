#include "MaterialInputNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/Compiler.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialInputNode::MaterialInputNode(int id, Graph* graph, asset::MaterialTypeAsset* material)
	: Node(id), m_material(material)
{
	// Create pins
    for (int i = 0; i < m_material->getFields().Properties.size(); i++) {
        asset::prop::Property* prop = m_material->getFields().Properties[i];
        node::OutputPin* opin = graph->createOutputPin(this, asset::prop::getNameForType(prop->getType()), prop->getName());
        p_outputs.push_back(
            opin
        );
    }
}

const char* MaterialInputNode::getNodeName() const {
    return "Material input";
}

void MaterialInputNode::renderBody() {
    ImGui::Text(m_material->getName().c_str());
    ImGui::Spacing();
}

bool MaterialInputNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "MaterialInput";
    out << YAML::Key << "Material" << YAML::Value << m_material->getName();
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

void MaterialInputNode::evaluate(compiler::GraphEvaluator* evaluator) {
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    for (int i = 0; i < m_material->getFields().Properties.size(); i++) {
        asset::prop::Property* prop = m_material->getFields().Properties[i];
        p_outputs[i]->setValue(se->getProperty(this->m_material, prop));
    }

    m_evaluated = true;
}

bool MaterialInputNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
