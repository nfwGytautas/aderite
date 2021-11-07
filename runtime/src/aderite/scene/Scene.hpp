#pragma once

#include <vector>

#include "aderite/Config.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Entity.hpp"

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
     * @brief Update scene
     * @param delta Delta time between frames
     */
    void update(float delta);

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

    /**
     * @brief Add entity to the scene
     * @param entity Entity to add
     */
    void addEntity(Entity* entity);

    /**
     * @brief Get entities in this scene
     */
    const std::vector<Entity*> getEntities() const {
        return m_entities;
    }

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class SceneManager;
    friend class SceneSerializer;

private:
    physics::PhysicsScene* m_physics = nullptr;
    rendering::Pipeline* m_pipeline = nullptr;
    std::vector<audio::AudioSource*> m_audioSources;
    std::vector<Entity*> m_entities;
};

} // namespace scene
} // namespace aderite
