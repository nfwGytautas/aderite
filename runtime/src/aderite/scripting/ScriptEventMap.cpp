#include "ScriptEventMap.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scripting {

#define SERIALIZE_EVENTS(rootName, objects)             \
    emitter << YAML::Key << rootName << YAML::BeginSeq; \
    for (const auto* e : m_updateEvents) {              \
        emitter << e->getFullName();                    \
    }                                                   \
    emitter << YAML::EndSeq;

// TODO: Check for failed parsing
#define DESERIALIZE_EVENTS(rootName, type)                                                                          \
    {                                                                                                               \
        auto objects = mapNode[rootName];                                                                           \
        if (objects) {                                                                                              \
            for (auto object : objects) {                                                                           \
                std::string fullName = object.as<std::string>();                                                    \
                scripting::ScriptEvent* parsed = ::aderite::Engine::getScriptManager()->getEventFromName(fullName); \
                if (parsed != nullptr) {                                                                            \
                    this->add(static_cast<type>(parsed));                                                           \
                }                                                                                                   \
            }                                                                                                       \
        }                                                                                                           \
    }

template<typename T>
void removeObject(std::vector<T*>& list, T* object) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Tried to remove nullptr object");
    auto it = std::find(list.begin(), list.end(), object);
    ADERITE_DYNAMIC_ASSERT(it != list.end(), "Tried to remove event that doesn't exist in the mapping");
    list.erase(it);
    delete object;
}

void ScriptEventMap::callUpdateEvents(float delta) {
    for (ScriptUpdateEvent* sue : m_updateEvents) {
        sue->call(delta);
    }
}

bool ScriptEventMap::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "ScriptEventMap" << YAML::BeginMap;

    SERIALIZE_EVENTS("Update", m_updateEvents);

    emitter << YAML::EndMap;

    return true;
}

bool ScriptEventMap::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& mapNode = data["ScriptEventMap"];
    if (!mapNode || mapNode.IsNull()) {
        return false;
    }

    DESERIALIZE_EVENTS("Update", scripting::ScriptUpdateEvent*);

    return true;
}

void ScriptEventMap::add(scripting::ScriptUpdateEvent* e) {
    m_updateEvents.push_back(e);
}

const std::vector<scripting::ScriptUpdateEvent*>& ScriptEventMap::getUpdateEvents() const {
    return m_updateEvents;
}

void ScriptEventMap::remove(scripting::ScriptUpdateEvent* e) {
    removeObject(m_updateEvents, e);
}

} // namespace scripting
} // namespace aderite
