#include "MaterialInputNode.hpp"

#include "aderite/Aderite.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialInputNode::MaterialInputNode()
{}

const char* MaterialInputNode::getNodeName() const {
    return "Material input";
}

void MaterialInputNode::renderBody() {
    //ImGui::Text(m_material->getName().c_str());
    //ImGui::Spacing();
}

void MaterialInputNode::evaluate(compiler::GraphEvaluator* evaluator) {
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    for (int i = 0; i < m_material->getFields().Properties.size(); i++) {
        asset::prop::Property* prop = m_material->getFields().Properties[i];
        p_outputs[i]->setValue(se->getProperty(this->m_material, prop));
    }

    m_evaluated = true;
}

io::SerializableType MaterialInputNode::getType() const {
    return static_cast<io::SerializableType>(io::EditorSerializables::MaterialInputNode);
}

bool MaterialInputNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    //emitter << YAML::Key << "Material" << YAML::Value << m_material->getName();
    serializeData(emitter);
    return true;
}

bool MaterialInputNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    //m_material = static_cast<asset::MaterialTypeAsset*>(::aderite::Engine::getAssetManager()->getOrRead(data["Material"].as<std::string>()));

    // Create pins
    for (int i = 0; i < m_material->getFields().Properties.size(); i++) {
        asset::prop::Property* prop = m_material->getFields().Properties[i];
        node::OutputPin* opin = new OutputPin(this, asset::prop::getNameForType(prop->getType()), prop->getName());
        p_outputs.push_back(
            opin
        );
    }

    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
