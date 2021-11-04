#include "ScriptComponents.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Audio.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scene/components/Transform.hpp"
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
    scene::Entity e = scene::Entity(entity, scene);
    if (e.hasComponent<scene::DynamicActor>() || e.hasComponent<scene::StaticActor>()) {
        LOG_ERROR("Don't directly set transform when a physics actor is attached, use PhysicsActor methods");
        return;
    }

    auto& transform = e.getComponent<scene::TransformComponent>();
    transform.Position = position;
}

void SetRotation(scene::Scene* scene, entt::entity entity, glm::quat rotation) {
    scene::Entity e = scene::Entity(entity, scene);
    if (e.hasComponent<scene::DynamicActor>() || e.hasComponent<scene::StaticActor>()) {
        LOG_ERROR("Don't directly set transform when a physics actor is attached, use PhysicsActor.Rotate");
        return;
    }

    auto& transform = e.getComponent<scene::TransformComponent>();
    transform.Rotation = rotation;
}

void SetScale(scene::Scene* scene, entt::entity entity, glm::vec3 scale) {
    scene::Entity e = scene::Entity(entity, scene);
    if (e.hasComponent<scene::DynamicActor>() || e.hasComponent<scene::StaticActor>()) {
        LOG_ERROR("Don't directly set transform when a physics actor is attached, use PhysicsActor.Scale");
        return;
    }

    auto& transform = e.getComponent<scene::TransformComponent>();
    transform.Scale = scale;
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

audio::AudioInstance* CreateInstance(scene::Scene* scene, entt::entity entity, asset::AudioAsset* audio) {
    scene::Entity e(entity, scene);
    if (!e.hasComponent<scene::AudioSourceComponent>()) {
        LOG_WARN("Tried to create audio instance on an entity that doesn't have a valid audio source");
    }

    audio::AudioInstance* instance = ::aderite::Engine::getAudioController()->createInstance(audio);
    e.getComponent<scene::AudioSourceComponent>().Request.Instances.push_back(instance);
    return instance;
}

void OneShot(scene::Scene* scene, entt::entity entity, asset::AudioAsset* audio) {
    scene::Entity e(entity, scene);
    if (!e.hasComponent<scene::AudioSourceComponent>()) {
        LOG_WARN("Tried to play one shot on an entity that doesn't have a valid audio source");
    }

    e.getComponent<scene::AudioSourceComponent>().Request.Oneshot.push_back(::aderite::Engine::getAudioController()->createOneshot(audio));
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

    // Audio
    mono_add_internal_call("Aderite.AudioSource::__CreateInstance(intptr,uintptr,intptr)", reinterpret_cast<void*>(CreateInstance));
    mono_add_internal_call("Aderite.AudioSource::__OneShot(intptr,uintptr,intptr)", reinterpret_cast<void*>(OneShot));
}

} // namespace scripting
} // namespace aderite
