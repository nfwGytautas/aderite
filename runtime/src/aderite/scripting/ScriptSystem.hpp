#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ThunkedMethod.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief ScriptSystem class is used to provide scripting engine the functionality to add it's own system that can handle any type of data
 */
class ScriptSystem final : public io::ISerializable {
public:
    ScriptSystem();

    /**
     * @brief Set the entity selector of the system, this will be able to reached through this.Entities field in the C# script
     * @param selector Selector instance to set
     */
    void setSelector(scene::EntitySelector* selector);

    /**
     * @brief Returns the entity selector instance of the system
     */
    scene::EntitySelector* getSelector() const {
        return m_selector;
    }

    /**
     * @brief Returns the name of the system
     */
    const std::string& getName() const {
        return m_name;
    }

    /**
     * @brief Update the system
     * @param delta Delta of the frame
     */
    void update(float delta);

    /**
     * @brief Initialize the system
     */
    void init();

    /**
     * @brief Shutdown the system
     */
    void shutdown();

    /**
     * @brief Invokes a OnTriggerEnter method with the specified trigger event object
     * @param triggerEvent TriggerEvent instance
     */
    void triggerEnter(MonoObject* triggerEvent);

    /**
     * @brief Invokes a OnTriggerLeave method with the specified trigger event object
     * @param triggerEvent TriggerEvent instance
     */
    void triggerLeave(MonoObject* triggerEvent);

    /**
     * @brief Invokes a OnCollisionStart method with the specified collision event object
     * @param collisionEvent CollisionEvent instance
     */
    void collisionStart(MonoObject* collisionEvent);

    /**
     * @brief Invokes a OnCollisionEnd method with the specified collision event object
     * @param collisionEvent CollisionEvent instance
     */
    void collisionEnd(MonoObject* collisionEvent);

    /**
     * @brief Loads the C# system instance into this instance
     * @param name Name of the system (including the namespace)
     */
    void load(const std::string& name);

    /**
     * @brief Attaches the system to the specified scene
     * @param scene Scene to atach to
     */
    void attachToScene(scene::Scene* scene);

    /**
     * @brief Returns the public fields of this system
     */
    const std::vector<FieldWrapper>& getFields() const {
        return m_fields;
    }

    /**
     * @brief Returns true if the system has a field with the specified name
     * @param name Name of the field
     */
    bool hasField(const std::string& name) const;

    /**
     * @brief Returns the field wrapper with the specified name
     * @param name Name of the wrapper
     */
    const FieldWrapper& getField(const std::string& name) const;

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    /**
     * @brief Updates the selector array
     */
    void updateEntitiesArray();

private:
    scene::EntitySelector* m_selector = nullptr;
    scene::Scene* m_scene = nullptr;
    std::string m_name = "";

    // Mono objects
    MonoClass* m_klass = nullptr;
    MonoObject* m_instance = nullptr;
    MonoArray* m_entities = nullptr;

    // System functions
    ThunkedMethod<void> m_init;
    ThunkedMethod<void, float, MonoArray*> m_update;
    ThunkedMethod<void> m_shutdown;

    ThunkedMethod<void, MonoObject*> m_triggerEnter;
    ThunkedMethod<void, MonoObject*> m_triggerLeave;
    ThunkedMethod<void, MonoObject*> m_collisionStart;
    ThunkedMethod<void, MonoObject*> m_collisionEnd;

    // User fields
    std::vector<FieldWrapper> m_fields;
};

} // namespace scripting
} // namespace aderite
