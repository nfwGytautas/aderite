#include "ScriptEventMap.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptSceneLoadedEvent.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scripting {

void ScriptEventMap::callUpdateEvents(float delta) {
    for (ScriptEvent* se : this->getEvents(ScriptEventType::UPDATE)) {
        static_cast<ScriptUpdateEvent*>(se)->call(delta);
    }
}

void ScriptEventMap::callSceneLoaded() {
    for (ScriptEvent* se : this->getEvents(ScriptEventType::SCENE_LOADED)) {
        static_cast<ScriptSceneLoadedEvent*>(se)->call();
    }
}

bool ScriptEventMap::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "ScriptEventMap" << YAML::BeginMap;

    emitter << YAML::Key << "Events" << YAML::BeginSeq;
    for (const auto* e : m_events) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(e->getEventType());
        emitter << YAML::Key << "Name" << YAML::Value << e->getFullName();
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    return true;
}

bool ScriptEventMap::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& mapNode = data["ScriptEventMap"];
    if (!mapNode || mapNode.IsNull()) {
        return false;
    }

    auto events = mapNode["Events"];
    if (events) {
        for (const YAML::Node& e : events) {
            const ScriptEventType type = static_cast<ScriptEventType>(e["Type"].as<size_t>());
            const std::string& name = e["Name"].as<std::string>();

            scripting::ScriptEvent* parsed = ::aderite::Engine::getScriptManager()->getEventFromName(name);
            if (parsed != nullptr) {
                this->add(parsed);
            }
        }
    }

    return true;
}

void ScriptEventMap::add(scripting::ScriptEvent* e) {
    m_events.push_back(e);
}

void ScriptEventMap::remove(scripting::ScriptEvent* e) {
    ADERITE_DYNAMIC_ASSERT(e != nullptr, "Tried to remove nullptr event");
    auto it = std::find(m_events.begin(), m_events.end(), e);
    ADERITE_DYNAMIC_ASSERT(it != m_events.end(), "Tried to remove event that doesn't exist in the mapping");
    m_events.erase(it);
    delete e;
}

const std::vector<scripting::ScriptEvent*>& ScriptEventMap::getEvents() const {
    return m_events;
}

std::vector<scripting::ScriptEvent*> ScriptEventMap::getEvents(ScriptEventType type) const {
    std::vector<scripting::ScriptEvent*> events;

    for (scripting::ScriptEvent* e : m_events) {
        if (e->getEventType() == type) {
            events.push_back(e);
        }
    }

    return events;
}

} // namespace scripting
} // namespace aderite
