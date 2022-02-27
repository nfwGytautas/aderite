#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief The main class used to represent an object in a scene
 */
class GameObject final : public io::SerializableObject {
public:
    /**
     * @brief Create an empty GameObject for the specified scene
     * @param scene Scene object to create in
     * @param name Name of the object
     */
    GameObject(scene::Scene* scene, const std::string& name);

    /**
     * @brief Destroys the GameObject instance and frees its memory
     */
    ~GameObject();

    /**
     * @brief Update scene
     * @param delta Delta time between frames
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
     * @brief Marks the game object for deletion
     */
    void markForDeletion();

    /**
     * @brief Returns true if the game object is marked to be deleted
     */
    bool isMarkedForDeletion() const;

    /**
     * @brief Returns the scene of the object
     */
    Scene* getScene() const;

    /**
     * @brief Returns the MonoObject instance of the object
     */
    MonoObject* getScriptInstance() const;

    /**
     * @brief Attach transform component to this GameObject
     */
    TransformProvider* addTransform();

    /**
     * @brief Remove transform component from this GameObject
     */
    void removeTransform();

    /**
     * @brief Returns the transform of this GameObject
     */
    TransformProvider* getTransform() const;

    /**
     * @brief Attach renderable component to this GameObject
     */
    rendering::Renderable* addRenderable();

    /**
     * @brief Remove renderable component from this GameObject
     */
    void removeRenderable();

    /**
     * @brief Returns the renderable of this GameObject
     */
    rendering::Renderable* getRenderable() const;

    /**
     * @brief Attach actor component to this GameObject
     */
    physics::PhysXActor* addActor();

    /**
     * @brief Remove actor component from this GameObject
     */
    void removeActor();

    /**
     * @brief Returns the actor of this GameObject
     */
    physics::PhysXActor* getActor() const;

    /**
     * @brief Attach camera component to this GameObject
     */
    Camera* addCamera();

    /**
     * @brief Remove camera component from this GameObject
     */
    void removeCamera();

    /**
     * @brief Returns the camera of this GameObject
     */
    Camera* getCamera() const;

    /**
     * @brief Attach audio source component to this GameObject
     */
    audio::AudioSource* addAudioSource();

    /**
     * @brief Remove audio source component from this GameObject
     */
    void removeAudioSource();

    /**
     * @brief Returns the audio source component of this GameObject
     */
    audio::AudioSource* getAudioSource() const;

    /**
     * @brief Attach audio listener component to this GameObject
     */
    audio::AudioListener* addAudioListener();

    /**
     * @brief Remove audio listener component from this GameObject
     */
    void removeAudioListener();

    /**
     * @brief Returns the audio source component of this GameObject
     */
    audio::AudioListener* getAudioListener() const;

    /**
     * @brief Adds behavior to this game object
     * @param behavior Behavior instance
     */
    void addBehavior(scripting::ScriptedBehavior* behavior);

    /**
     * @brief Remove behavior from this game object
     * @param behavior Behavior to remove
     */
    void removeBehavior(scripting::ScriptedBehavior* behavior);

    /**
     * @brief Returns the behaviors of this game object
     */
    std::vector<scripting::ScriptedBehavior*> getBehaviors() const;

    // Inherited via SerializableObject
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    reflection::Type getType() const override;

private:
    // Common
    Scene* m_scene = nullptr;
    bool m_markedForDeletion = false;

    // Components
    TransformProvider* m_transform = nullptr;
    rendering::Renderable* m_renderable = nullptr;
    physics::PhysXActor* m_actor = nullptr;
    Camera* m_camera = nullptr;
    audio::AudioSource* m_audioSource = nullptr;
    audio::AudioListener* m_audioListener = nullptr;
    std::vector<scripting::ScriptedBehavior*> m_behaviors;

    // Scripting instance
    MonoObject* m_instance = nullptr;
};

} // namespace scene
} // namespace aderite
