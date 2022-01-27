#pragma once

#include <string>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"

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
     * @brief Attach transform component to this GameObject
     */
    void addTransform();

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
    void addRenderable();

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
    void addActor();

    /**
     * @brief Remove actor component from this GameObject
     */
    void removeActor();

    /**
     * @brief Returns the actor of this GameObject
     */
    physics::PhysXActor* getActor();

    /**
     * @brief Attach camera component to this GameObject
     */
    void addCamera();

    /**
     * @brief Remove camera component from this GameObject
     */
    void removeCamera();

    /**
     * @brief Returns the camera of this GameObject
     */
    Camera* getCamera();

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
};

} // namespace scene
} // namespace aderite
