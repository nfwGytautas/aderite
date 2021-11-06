#include "ScriptList.hpp"

#include "aderite/io/Serializer.hpp"
#include "aderite/scripting/Script.hpp"

namespace aderite {
namespace scripting {

ScriptList::ScriptList() {}

ScriptList::~ScriptList() {
    for (Script* s : m_scripts) {
        delete s;
    }
}

void ScriptList::update(float delta) const {
    for (Script* s : m_scripts) {
        s->update(delta);
    }
}

void ScriptList::addScript(Script* script) {
    m_scripts.push_back(script);
}

void ScriptList::removeScript(Script* script) {
    auto it = std::find_if(m_scripts.begin(), m_scripts.end(), [script](Script* s) {
        return script == s;
    });

    ADERITE_DYNAMIC_ASSERT(it != m_scripts.end(), "Trying to remove a script that doesn't exist in the list");
    m_scripts.erase(it);
}

void ScriptList::pair(scene::Entity entity) {
    for (Script* s : m_scripts) {
        s->pair(entity);
    }
}

void ScriptList::onTriggerEnter(scene::Entity trigger) {
    for (Script* s : m_scripts) {
        s->onTriggerEnter(trigger);
    }
}

void ScriptList::onTriggerLeave(scene::Entity trigger) {
    for (Script* s : m_scripts) {
        s->onTriggerLeave(trigger);
    }
}

void ScriptList::onCollisionEnter(scene::Entity collision) {
    for (Script* s : m_scripts) {
        s->onCollisionEnter(collision);
    }
}

void ScriptList::onCollisionLeave(scene::Entity collision) {
    for (Script* s : m_scripts) {
        s->onCollisionLeave(collision);
    }
}

reflection::Type ScriptList::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCRIPT_LIST);
}

bool ScriptList::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Scripts";
    emitter << YAML::BeginSeq;
    for (Script* s : m_scripts) {
        serializer->writeUntrackedType(emitter, s);
    }
    emitter << YAML::EndSeq;
    return true;
}

bool ScriptList::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    for (const YAML::Node& scriptNode : data["Scripts"]) {
        Script* script = static_cast<Script*>(serializer->parseUntrackedType(scriptNode));
        if (script->getBehavior() != nullptr) {
            addScript(script);
        }
    }
    return true;
}

} // namespace scripting
} // namespace aderite
