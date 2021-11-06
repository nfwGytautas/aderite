#pragma once

#include <vector>

#include <entt/entity/registry.hpp>

#include "aderite/Config.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Meta.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
 */
class Scene : public io::SerializableObject {
public:
    Scene();
    virtual ~Scene();

    /**
     * @brief Returns entt registry
     */
    entt::registry& getEntityRegistry() {
        return m_registry;
    }

    const entt::registry& getEntityRegistry() const {
        return m_registry;
    }

    /**
     * @brief Update scene
     * @param delta Delta time between frames
     */
    void update(float delta);

    /**
     * @brief Create Entity with a MetaComponent component
     * @param meta MetaComponent of the entity
     * @return Entity instance
     */
    Entity createEntity(const MetaComponent& meta);

    /**
     * @brief Destroy an Entity
     */
    void destroyEntity(Entity entity);

    /**
     * @brief Adds an AudioSource to the scene
     * @param source Source to add
    */
    void addSource(audio::AudioSource* source);

    /**
     * @brief Returns the pipeline of this scene
     */
    rendering::Pipeline* getPipeline() const;

    /**
     * @brief Returns the physics scene attached to this one
     */
    physics::PhysicsScene* getPhysicsScene() const;

    /**
     * @brief Returns audio sources of this scene
    */
    const std::vector<audio::AudioSource*>& getAudioSources() const {
        return m_audioSources;
    }

    /**
     * @brief Sets the pipeline of the scene
     * @param pipeline New pipeline
     */
    void setPipeline(rendering::Pipeline* pipeline);

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    /**
     * @brief Function invoked right after a component was added to entity
     */
    template<typename T>
    void onComponentAdded(Entity entity, T& component);

    /**
     * @brief Function invoked right before a component is removed from entity
     */
    template<typename T>
    void onComponentRemoved(Entity entity, T& component);

    friend class Entity;
    friend class SceneManager;
    friend class SceneSerializer;

private:
    entt::registry m_registry;
    physics::PhysicsScene* m_physics = nullptr;

    rendering::Pipeline* m_pipeline = nullptr;
    std::vector<audio::AudioSource*> m_audioSources;
};

} // namespace scene
} // namespace aderite
