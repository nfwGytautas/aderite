#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Entity class is used to represent an object in the world
 */
class Entity : public io::ISerializable {
public:
    Entity();
    virtual ~Entity();

    /**
     * @brief Adds a tag to the entity
     * @param tag Tag to add
     */
    void addTag(size_t tag);

    /**
     * @brief Remove tag from the entity
     * @param tag Tag to remove
     */
    void removeTag(size_t tag);

    /**
     * @brief Returns true if the entity has the specified tag, false otherwise
     */
    bool hasTag(size_t tag) const;

    /**
     * @brief Set the scene of the entity, this will transfer actor to the new scene
     * @param scene New scene
     */
    void setScene(Scene* scene);

    /**
     * @brief Set the actor for the entity
     * @param actor Physics actor instance
     * @param keepColliders If true then colliders will be moved from previous actor to the new one
     */
    void setActor(physics::PhysicsActor* actor, bool keepColliders = false);

    /**
     * @brief Set the renderable for the entity
     * @param renderable Renderable instance
     */
    void setRenderable(rendering::Renderable* renderable);

    /**
     * @brief Set the name of the entity
     * @param name New name of the entity
     */
    void setName(const std::string& name);

    /**
     * @brief Returns the scene of the entity
     */
    Scene* getScene() const {
        return m_scene;
    }

    /**
     * @brief Returns the transform of the entity
     */
    Transform* getTransform() const {
        return m_transform;
    }

    /**
     * @brief Returns the physics actor of the entity
     */
    physics::PhysicsActor* getActor() const {
        return m_actor;
    }

    /**
     * @brief Returns the renderable of this entity
     */
    rendering::Renderable* getRenderable() const {
        return m_renderable;
    }

    /**
     * @brief Returns the tagset of this entity
     */
    const size_t getTags() const {
        return m_tags;
    }

    /**
     * @brief Returns the name of this entity
     */
    const std::string& getName() const {
        return m_name;
    }

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class Scene;

private:
    /**
     * @brief Scene this entity belongs to
     */
    Scene* m_scene = nullptr;

    /**
     * @brief Transform of the entity
     */
    Transform* m_transform = nullptr;

    /**
     * @brief Tag bitset
     */
    size_t m_tags = 0;

    /**
     * @brief Name of the entity
     */
    std::string m_name = "";

    /**
     * @brief Physics actor of the entity
     */
    physics::PhysicsActor* m_actor = nullptr;

    /**
     * @brief Renderable of the entity
     */
    rendering::Renderable* m_renderable = nullptr;
};

} // namespace scene
} // namespace aderite
