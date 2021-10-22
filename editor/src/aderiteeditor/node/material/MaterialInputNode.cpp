#include "MaterialInputNode.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/vfs/File.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialInputNode::MaterialInputNode()
{}

void MaterialInputNode::setType(asset::EditorMaterialType* type) {
    for (OutputPin* opin : p_outputs) {
        opin->disconnect();
        delete opin;
    }
    p_outputs.clear();

    m_material = type;
    generatePins();
}

const char* MaterialInputNode::getNodeName() const {
    return "Material input";
}

void MaterialInputNode::renderBody() {
    static vfs::File* file = editor::State::Project->getVfs()->getFile(m_material->getHandle());
    ImGui::Text(file->getName().c_str());
    ImGui::Spacing();
}

void MaterialInputNode::evaluate(compiler::GraphEvaluator* evaluator) {
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    auto props = m_material->getProperties();
    for (int i = 0; i < props.size(); i++) {
        asset::Property* prop = props[i];
        p_outputs[i]->setValue(se->getProperty(this->m_material, prop));
    }

    m_evaluated = true;
}

reflection::Type MaterialInputNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::MaterialInputNode);
}

bool MaterialInputNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "Material" << YAML::Value << m_material->getHandle();
    serializeData(emitter);
    return true;
}

bool MaterialInputNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
    m_material = static_cast<asset::EditorMaterialType*>(
        ::aderite::Engine::getSerializer()->getOrRead(data["Material"].as<io::SerializableHandle>()));

    generatePins();

    deserializeData(data);
    return true;
}

void MaterialInputNode::generatePins() {
    // Create pins
    auto props = m_material->getProperties();
    for (int i = 0; i < props.size(); i++) {
        asset::Property* prop = props[i];
        node::OutputPin* opin = new OutputPin(this, asset::getNameForType(prop->getType()), prop->getName());
        p_outputs.push_back(
            opin
        );
    }
}

ADERITE_EDITOR_NODE_NAMESPACE_END
