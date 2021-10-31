#include "ScriptComponents.hpp"

#include <mono/jit/jit.h>

#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

glm::vec3 GetPosition(scene::Scene* scene, entt::entity entity) {
    return scene::Entity(entity, scene).getComponent<scene::TransformComponent>().Position;
}

glm::quat GetRotation(scene::Scene* scene, entt::entity entity) {
    return scene::Entity(entity, scene).getComponent<scene::TransformComponent>().Rotation;
}

glm::vec3 GetScale(scene::Scene* scene, entt::entity entity) {
    return scene::Entity(entity, scene).getComponent<scene::TransformComponent>().Scale;
}

void SetPosition(scene::Scene* scene, entt::entity entity, glm::vec3 position) {
    auto& transform = scene::Entity(entity, scene).getComponent<scene::TransformComponent>();
    transform.Position = position;
    transform.WasAltered = true;
}

void SetRotation(scene::Scene* scene, entt::entity entity, glm::quat rotation) {
    auto& transform = scene::Entity(entity, scene).getComponent<scene::TransformComponent>();
    transform.Rotation = rotation;
    transform.WasAltered = true;
}

void SetScale(scene::Scene* scene, entt::entity entity, glm::vec3 scale) {
    auto& transform = scene::Entity(entity, scene).getComponent<scene::TransformComponent>();
    transform.Scale = scale;
    transform.WasAltered = true;
}

asset::MeshAsset* GetMesh(scene::Scene* scene, entt::entity entity) {
    return scene::Entity(entity, scene).getComponent<scene::MeshRendererComponent>().MeshHandle;
}

asset::MaterialAsset* GetMaterial(scene::Scene* scene, entt::entity entity) {
    return scene::Entity(entity, scene).getComponent<scene::MeshRendererComponent>().MaterialHandle;
}

void SetMesh(scene::Scene* scene, entt::entity entity, asset::MeshAsset* mesh) {
    scene::Entity(entity, scene).getComponent<scene::MeshRendererComponent>().MeshHandle = mesh;
}

void SetMaterial(scene::Scene* scene, entt::entity entity, asset::MaterialAsset* material) {
    scene::Entity(entity, scene).getComponent<scene::MeshRendererComponent>().MaterialHandle = material;
}

void componentInternals() {
    // Transform
    mono_add_internal_call("Aderite.Transform::__GetPosition(intptr,uintptr)", reinterpret_cast<void*>(GetPosition));
    mono_add_internal_call("Aderite.Transform::__GetRotation(intptr,uintptr)", reinterpret_cast<void*>(GetRotation));
    mono_add_internal_call("Aderite.Transform::__GetScale(intptr,uintptr)", reinterpret_cast<void*>(GetScale));

    mono_add_internal_call("Aderite.Transform::__SetPosition(intptr,uintptr,Aderite.Vector3)", reinterpret_cast<void*>(SetPosition));
    mono_add_internal_call("Aderite.Transform::__SetRotation(intptr,uintptr,Aderite.Quaternion)", reinterpret_cast<void*>(SetRotation));
    mono_add_internal_call("Aderite.Transform::__SetScale(intptr,uintptr,Aderite.Vector3)", reinterpret_cast<void*>(SetScale));

    // Mesh renderer
    mono_add_internal_call("Aderite.MeshRenderer::__GetMesh(intptr,uintptr)", reinterpret_cast<void*>(GetMesh));
    mono_add_internal_call("Aderite.MeshRenderer::__GetMaterial(intptr,uintptr)", reinterpret_cast<void*>(GetMaterial));

    mono_add_internal_call("Aderite.MeshRenderer::__SetMesh(intptr,uintptr,intptr)", reinterpret_cast<void*>(SetMesh));
    mono_add_internal_call("Aderite.MeshRenderer::__SetMaterial(intptr,uintptr,intptr)", reinterpret_cast<void*>(SetMaterial));
}

} // namespace scripting
} // namespace aderite
