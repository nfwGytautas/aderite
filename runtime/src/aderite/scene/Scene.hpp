#pragma once

#include <vector>

#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
 */
class Scene final : public io::SerializableObject {
    static constexpr size_t c_MaxTags = sizeof(size_t) * 8; // Bitflag

public:
    Scene();
    ~Scene();

    /**
     * @brief Update scene
     * @param delta Delta time between frames
     */
    void update(float delta);

    /**
     * @brief Add a visual to the scene (scene takes ownership of the visual)
     * @param visual Visual to add
     */
    void addVisual(Visual* visual);

    /**
     * @brief Remove visual from the scene
     * @param visual Visual to remove
     */
    void removeVisual(Visual* visual);

    /**
     * @brief Returns the visuals of this scene
     */
    const std::vector<Visual*> getVisuals() const;

    /**
     * @brief Add a scenery to the scene (scene takes ownership of the scenery)
     * @param scenery Scenery to add
     */
    void addScenery(Scenery* scenery);

    /**
     * @brief Remove visual from the scene
     * @param scenery Scenery to remove
     */
    void removeScenery(Scenery* scenery);

    /**
     * @brief Returns the scenery of this scene
     */
    const std::vector<Scenery*> getScenery() const;

    /**
     * @brief Adds a script system to the scene
     * @param system System to add
     */
    void addScriptSystem(scripting::ScriptSystem* system);

    /**
     * @brief Adds a audio listener to the scene
     * @param listener AudioListener instance
     */
    void addAudioListener(audio::AudioListener* listener);

    /**
     * @brief Adds a audio source to the scene
     * @param source AudioSource instance
     */
    void addAudioSource(audio::AudioSource* source);

    /**
     * @brief Sets the pipeline of the scene
     * @param pipeline New pipeline
     */
    void setPipeline(rendering::Pipeline* pipeline);

    /**
     * @brief Add tag to the scene
     * @param name
     */
    void addTag(const std::string& name);

    /**
     * @brief Removes tag from the scene
     * @param name
     */
    void removeTag(const std::string& name);

    /**
     * @brief Returns the number of empty tag slots
     */
    size_t getFreeTagSlots() const;

    /**
     * @brief Returns the index of the specified tag
     * @param name Name of the tag
     */
    size_t getTagIndex(const std::string& name) const;

    /**
     * @brief Returns the pipeline of this scene
     */
    rendering::Pipeline* getPipeline() const;

    /**
     * @brief Returns the physics scene attached to this one
     */
    physics::PhysicsScene* getPhysicsScene() const;

    /**
     * @brief Returns the audio source with the specified name
     * @param name Name of the source
     * @return AudioSource instance or nullptr if not found
     */
    audio::AudioSource* getSource(const std::string& name) const;

    /**
     * @brief Returns audio sources of this scene
     */
    const std::vector<audio::AudioSource*>& getAudioSources() const;

    /**
     * @brief Returns audio listeners of this scene
     */
    const std::vector<audio::AudioListener*>& getAudioListeners() const;

    /**
     * @brief Returns script systems active in this scene
     */
    const std::vector<scripting::ScriptSystem*> getScriptSystems() const;

    /**
     * @brief Returns the tags of the scene
     */
    const std::vector<std::string>& getTags() const;

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class SceneManager;
    friend class SceneSerializer;

private:
    physics::PhysicsScene* m_physics = nullptr;
    rendering::Pipeline* m_pipeline = nullptr;

    std::vector<Visual*> m_visuals;
    std::vector<Scenery*> m_scenery;

    std::vector<audio::AudioSource*> m_audioSources;
    std::vector<audio::AudioListener*> m_audioListeners;

    std::vector<scripting::ScriptSystem*> m_systems;
    std::vector<std::string> m_tags;
};

} // namespace scene
} // namespace aderite
