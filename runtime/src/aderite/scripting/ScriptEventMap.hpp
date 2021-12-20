#pragma once

#include <vector>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ScriptEventType.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief ScriptEventMap class is used to establish a connection between scriptable C# objects and aderite engine events such as input key
 * press or timed events such as update
 */
class ScriptEventMap : public io::ISerializable {
    // Generic function
public:
    virtual ~ScriptEventMap() = default;

    /**
     * @brief Invoke all update events
     * @param delta Delta to pass
     */
    void callUpdateEvents(float delta);

    /**
     * @brief Invoked when the scene has been loaded
     */
    void callSceneLoaded();

protected:
    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

public:
    /**
     * @brief Adds a script event
     * @param e Event instance
     */
    void add(scripting::ScriptEvent* e);

    /**
     * @brief Remove script event from mapping
     * @param e Event to remove
     */
    void remove(scripting::ScriptEvent* e);

    /**
     * @brief Returns all events
     */
    const std::vector<scripting::ScriptEvent*>& getEvents() const;

    /**
     * @brief Returns all events of the specified type
     * @param type Type of events to return
     */
    std::vector<scripting::ScriptEvent*> getEvents(ScriptEventType type) const;

private:
    std::vector<scripting::ScriptEvent*> m_events;
};

} // namespace scripting
} // namespace aderite
