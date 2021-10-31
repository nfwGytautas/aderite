#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
 */

#include <string>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/Handles.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Component containing all MetaComponent information about an Entity, such as it's name, tags, etc.
 */
struct MetaComponent {
    std::string Name;
    entt::entity This;

    MetaComponent() = default;
    MetaComponent(const MetaComponent&) = default;
    MetaComponent(const std::string& name) : Name(name) {}
};

/**
 * @brief Transform component, used to situate an Entity in world space
 */
struct TransformComponent {
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::quat Rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    /**
     * @brief Flag specifying if the transform component was updated in this cycle
     */
    bool WasAltered = true;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) :
        Position(position),
        Rotation(rotation),
        Scale(scale) {}

    /**
     * @brief Computes the transformation matrix of the component
     * @param t Transform component
     * @return Computed transformation matrix
     */
    static glm::mat4 computeTransform(const TransformComponent& t) {
        glm::mat4 rotation = glm::toMat4(t.Rotation);
        return glm::translate(glm::mat4(1.0f), t.Position) * rotation * glm::scale(glm::mat4(1.0f), t.Scale);
    }
};

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
 * @brief Rigid body component used for physics
 */
struct RigidbodyComponent {
    bool IsKinematic = false;
    bool HasGravity = true;
    float Mass = 1.0f;

    /**
     * @brief Flag specifying if the transform component was updated in this cycle
     */
    bool WasAltered = true;

    RigidbodyComponent() = default;
    RigidbodyComponent(const RigidbodyComponent&) = default;
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
