#pragma once

#include <mono/jit/jit.h>

#include "aderite/io/ISerializable.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap around a scripted behavior component
 */
class ScriptedBehavior final : public io::ISerializable {
public:
    ScriptedBehavior(BehaviorBase* behavior, scene::GameObject* gObject);

    /**
     * @brief Initializes scripted behavior
     */
    void init();

    /**
     * @brief Shuts down scripted behavior
     */
    void shutdown();

    /**
     * @brief Performs a script update
     * @param delta Delta time of the frame
     */
    void update(float delta);

    /**
     * @brief Function called when this game object enters another trigger
     * @param te Trigger event
     */
    void onTriggerEnter(const physics::TriggerEvent& te);

    /**
     * @brief Function called when this game object leaves another trigger
     * @param te Trigger event
     */
    void onTriggerLeave(const physics::TriggerEvent& te);

    /**
     * @brief Function called when the game object enters this trigger zone
     * @param te Trigger event
     */
    void onTriggerWasEntered(const physics::TriggerEvent& te);

    /**
     * @brief Function called when the game object leaves this trigger zone
     * @param te Trigger event
     */
    void onTriggerWasLeft(const physics::TriggerEvent& te);

    /**
     * @brief Function called when the game object enters a collision
     * @param ce Collision event
     */
    void onCollisionEnter(const physics::CollisionEvent& ce);

    /**
     * @brief Function called when the game object leaves a collision
     * @param ce Collision event
     */
    void onCollisionLeave(const physics::CollisionEvent& ce);

    /**
     * @brief Returns the behavior base
     */
    BehaviorBase* getBase() const;

    /**
     * @brief Returns the C# instance of this object
     */
    MonoObject* getInstance() const;

    /**
     * @brief Returns the game object that this script is attached to
     */
    scene::GameObject* getGameObject() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::GameObject* m_gameObject = nullptr;
    BehaviorBase* m_behaviorBase = nullptr;
    MonoObject* m_instance = nullptr;

    bool m_initialized = false;
};

} // namespace scripting
} // namespace aderite
