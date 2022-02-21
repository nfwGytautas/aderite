#pragma once

#include <memory>
#include <vector>

#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/physics/PhysicsScene.hpp"
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
class Scene final : public io::SerializableAsset, public physics::PhysicsScene {
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
     * @brief Creates a game object from a prefab
     * @param prefab Prefab to create from
     * @return New game object instance
     */
    GameObject* createGameObject(asset::PrefabAsset* prefab);

    /**
     * @brief Destroy the specified game object
     * @param object Object to destroy
     */
    void destroyGameObject(GameObject* object);

    /**
     * @brief Returns the game objects in this scene
     */
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const;

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class SceneManager;
    friend class SceneSerializer;

private:
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};

} // namespace scene
} // namespace aderite
