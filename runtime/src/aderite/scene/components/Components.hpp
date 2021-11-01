#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
 */


#include <glm/glm.hpp>

#include "aderite/Handles.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Mesh rendering component, this component contains information needed for rendering meshes
 */
struct MeshRendererComponent {
    asset::MeshAsset* MeshHandle = nullptr;
    asset::MaterialAsset* MaterialHandle = nullptr;

    MeshRendererComponent() = default;
    MeshRendererComponent(const MeshRendererComponent&) = default;
    MeshRendererComponent(asset::MeshAsset* mesh, asset::MaterialAsset* material) : MeshHandle(mesh), MaterialHandle(material) {}
};

/**
 * @brief Camera component, which references ICamera object
 */
struct CameraComponent {
    bool Main = true;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

/**
 * @brief Component containing all colliders attached to this object
 */
struct CollidersComponent {
    physics::ColliderList* Colliders = nullptr;

    CollidersComponent() = default;
    CollidersComponent(const CollidersComponent&) = default;
};

/**
 * @brief Component containing information about audio source on an entity (1 per entity)
 */
struct AudioSourceComponent {
    audio::AudioInstanceId Instance = c_InvalidHandle;
    bool PlayOnStart = false;

    AudioSourceComponent() = default;
    AudioSourceComponent(const AudioSourceComponent&) = default;
};

/**
 * @brief Component used to specify that this entity is listening to audio inputs
 */
struct AudioListenerComponent {
    bool IsEnabled = false;

    AudioListenerComponent() = default;
    AudioListenerComponent(const AudioListenerComponent&) = default;
};

/**
 * @brief Component used to specify that this entity has ScriptableBehaviors applied to it
 */
struct ScriptsComponent {
    scripting::ScriptList* Scripts = nullptr;

    ScriptsComponent() = default;
    ScriptsComponent(const ScriptsComponent&) = default;
};

} // namespace scene
} // namespace aderite
