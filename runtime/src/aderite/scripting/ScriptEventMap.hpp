#pragma once

#include <vector>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scripting/Forward.hpp"

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

protected:
    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Input events
public:
    // Engine events
public:
    /**
     * @brief Adds an update event to be called on engine update
     * @param e Event to call
     */
    void add(scripting::ScriptUpdateEvent* e);

    /**
     * @brief Returns the update events
     */
    const std::vector<scripting::ScriptUpdateEvent*>& getUpdateEvents() const;

    /**
     * @brief Remove script update event from mapping
     * @param e Event to remove
    */
    void remove(scripting::ScriptUpdateEvent* e);

private:
    std::vector<scripting::ScriptUpdateEvent*> m_updateEvents;
};

} // namespace scripting
} // namespace aderite
