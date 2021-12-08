#pragma once

#include <vector>

#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ScriptEventMap.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
 */
class Scene final : public io::SerializableAsset, public physics::PhysicsScene, public scripting::ScriptEventMap {
public:
    Scene();
    virtual ~Scene();

    /**
     * @brief Update scene
     * @param delta Delta time between frames
     */
    void update(float delta);

    /**
     * @brief Add a visual to the scene (takes ownership)
     * @param visual Visual to add
     */
    void add(Visual* visual);

    /**
     * @brief Remove visual from the scene
     * @param visual Visual to remove
     */
    void remove(Visual* visual);

    /**
     * @brief Returns the visuals of this scene
     */
    const std::vector<Visual*> getVisuals() const;

    /**
     * @brief Add a scenery to the scene (takes ownership)
     * @param scenery Scenery to add
     */
    void add(Scenery* scenery);

    /**
     * @brief Remove scenery from the scene
     * @param scenery Scenery to remove
     */
    void remove(Scenery* scenery);

    /**
     * @brief Returns the scenery of this scene
     */
    const std::vector<Scenery*> getScenery() const;

    /**
     * @brief Add a entity to the scene (takes ownership)
     * @param entity Entity to add
     */
    void add(Entity* entity);

    /**
     * @brief Remove entity from the scene
     * @param entity Entity to remove
     */
    void remove(Entity* entity);

    /**
     * @brief Returns the entities of this scene
     */
    const std::vector<Entity*> getEntities() const;

    /**
     * @brief Add a static physics region to the scene (takes ownership)
     * @param region StaticPhysicsRegion to add
     */
    void add(StaticPhysicsRegion* region);

    /**
     * @brief Remove static physics region from the scene
     * @param region StaticPhysicsRegion to remove
     */
    void remove(StaticPhysicsRegion* region);

    /**
     * @brief Returns the static physics regions of this scene
     */
    const std::vector<StaticPhysicsRegion*> getStaticPhysicsRegions() const;

    /**
     * @brief Add a physics region to the scene (takes ownership)
     * @param region DynamicPhysicsRegion to add
     */
    void add(DynamicPhysicsRegion* region);

    /**
     * @brief Remove physics region from the scene
     * @param region DynamicPhysicsRegion to remove
     */
    void remove(DynamicPhysicsRegion* region);

    /**
     * @brief Returns the dynamic physics regions of this scene
     */
    const std::vector<DynamicPhysicsRegion*> getDynamicPhysicsRegions() const;

    /**
     * @brief Add a audio listener to the scene (takes ownership)
     * @param listener AudioListener instance
     */
    void add(audio::AudioListener* listener);

    /**
     * @brief Remove listener from the scene
     * @param listener Listener to remove
     */
    void remove(audio::AudioListener* listener);

    /**
     * @brief Returns audio listeners of this scene
     */
    const std::vector<audio::AudioListener*>& getAudioListeners() const;

    /**
     * @brief Add a audio source to the scene (takes ownership)
     * @param source AudioSource instance
     */
    void add(audio::AudioSource* source);

    /**
     * @brief Remove source from the scene
     * @param source Source to remove
     */
    void remove(audio::AudioSource* source);

    /**
     * @brief Returns audio sources of this scene
     */
    const std::vector<audio::AudioSource*>& getAudioSources() const;

    /**
     * @brief Add a camera to the scene (takes ownership)
     * @param camera Camera instance
     */
    void add(Camera* camera);

    /**
     * @brief Remove camera from the scene
     * @param camera Camera to remove
     */
    void remove(Camera* camera);

    /**
     * @brief Returns cameras of this scene
     */
    const std::vector<Camera*>& getCameras() const;

    /**
     * @brief Returns the audio source with the specified name
     * @param name Name of the source
     * @return AudioSource instance or nullptr if not found
     */
    audio::AudioSource* getSource(const std::string& name) const;

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class SceneManager;
    friend class SceneSerializer;

private:
    // Objects
    std::vector<Visual*> m_visuals;
    std::vector<Scenery*> m_scenery;
    std::vector<Entity*> m_entities;

    // Physics
    std::vector<StaticPhysicsRegion*> m_staticPhysicsRegions;
    std::vector<DynamicPhysicsRegion*> m_dynamicPhysicsRegions;

    // Audio
    std::vector<audio::AudioSource*> m_audioSources;
    std::vector<audio::AudioListener*> m_audioListeners;

    // Other
    std::vector<Camera*> m_cameras;
};

} // namespace scene
} // namespace aderite
