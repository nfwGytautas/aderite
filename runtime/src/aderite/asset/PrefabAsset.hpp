#pragma once

#include <vector>

#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Class representing a prefab asset with it's own properties
 */
class PrefabAsset final : public io::SerializableAsset {
public:
    PrefabAsset();
    PrefabAsset(scene::GameObject* gObject);
    ~PrefabAsset();

    /**
     * @brief Create a game object from this prefab in a specified scene
     * @param scene Scene to create game object in
     * @return Created game object instance
     */
    scene::GameObject* instantiate(scene::Scene* scene) const;

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::TransformProvider* m_transform = nullptr;
    rendering::RenderableData* m_renderable = nullptr;
    physics::PhysicsProperties* m_actor = nullptr;
    scene::CameraSettings* m_camera = nullptr;
    audio::AudioListenerData* m_audioListener = nullptr;
    audio::AudioSourceData* m_audioSource = nullptr;
    std::vector<scripting::ScriptedBehavior*> m_behaviors;
};

} // namespace asset
} // namespace aderite
