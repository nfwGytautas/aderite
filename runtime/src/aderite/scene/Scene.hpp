#pragma once

#include <memory>
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
     * @brief Creates an empty game object
     * @return New game object instance
     */
    GameObject* createGameObject();

    /**
     * @brief Destroy the specified game object
     * @param object Object to destroy
     */
    void destroyGameObject(GameObject* object);

    /**
     * @brief Returns the game objects in this scene
     */
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const;

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
    const std::vector<std::unique_ptr<audio::AudioListener>>& getAudioListeners() const;

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
    const std::vector<std::unique_ptr<audio::AudioSource>>& getAudioSources() const;

    /**
     * @brief Adds/removes new or deleted scripts from this scene
     */
    void updateScriptDataEntries();

    /**
     * @brief Returns script data of this scene
     */
    const std::vector<std::unique_ptr<scripting::ScriptData>>& getScriptData() const;

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
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;

    // Audio
    std::vector<std::unique_ptr<audio::AudioSource>> m_audioSources;
    std::vector<std::unique_ptr<audio::AudioListener>> m_audioListeners;

    // Other
    std::vector<std::unique_ptr<scripting::ScriptData>> m_scriptData;
};

} // namespace scene
} // namespace aderite
