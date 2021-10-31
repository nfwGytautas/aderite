#include "MaterialInputNode.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

namespace aderite {
namespace node {

MaterialInputNode::MaterialInputNode() {}

void MaterialInputNode::setType(asset::EditorMaterialType* type) {
    for (OutputPin* opin : p_outputs) {
        opin->disconnect();
        delete opin;
    }

    m_material = type;
    generatePins();
}

const char* MaterialInputNode::getNodeName() const {
    return "Material input";
}

void MaterialInputNode::renderBody() {
    static vfs::File* file = editor::State::Project->getVfs()->getFile(m_material->getHandle());

    auto props = m_material->getProperties();
    auto samplers = m_material->getSamplers();
    if (props.size() + samplers.size() != p_outputs.size()) {
        generatePins();
    } else {
        for (size_t i = 0; i < props.size(); i++) {
            p_outputs[i]->setName(props[i]->getName());
        }

        for (size_t i = props.size(); i < props.size() + samplers.size(); i++) {
            p_outputs[i]->setName(samplers[i - props.size()]->getName());
        }
    }

    ImGui::Text(file->getName().c_str());
    ImGui::Spacing();
}

void MaterialInputNode::evaluate(compiler::GraphEvaluator* evaluator) {
    compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
    auto props = m_material->getProperties();
    for (int i = 0; i < props.size(); i++) {
        asset::Property* prop = props[i];
        p_outputs[i]->setValue(se->getProperty(prop));
    }

    auto samps = m_material->getSamplers();
    for (int i = props.size(); i < samps.size() + props.size(); i++) {
        asset::Sampler* samp = samps[i - props.size()];
        p_outputs[i]->setValue(se->getSampler(samp));
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

bool MaterialInputNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_material = static_cast<asset::EditorMaterialType*>(
        ::aderite::Engine::getSerializer()->getOrRead(data["Material"].as<io::SerializableHandle>()));

    generatePins();

    deserializeData(data);
    return true;
}

void MaterialInputNode::generatePins() {
    // Create pins
    p_outputs.clear();
    auto props = m_material->getProperties();
    for (int i = 0; i < props.size(); i++) {
        asset::Property* prop = props[i];
        node::OutputPin* opin = new OutputPin(this, asset::getNameForType(prop->getType()), prop->getName());
        p_outputs.push_back(opin);
    }

    auto samplers = m_material->getSamplers();
    for (int i = 0; i < samplers.size(); i++) {
        asset::Sampler* samp = samplers[i];
        node::OutputPin* opin = new OutputPin(this, asset::getNameForType(samp->getType()), samp->getName());
        p_outputs.push_back(opin);
    }
}

} // namespace node
} // namespace aderite
