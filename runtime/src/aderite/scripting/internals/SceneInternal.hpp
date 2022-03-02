#pragma once

#ifndef ADERITE_SCENE_INTERNALS
#define ADERITE_SCENE_INTERNALS
#else
#error "Multiple SceneInternal.hpp include"
#endif

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/scripting/BehaviorBase.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptedBehavior.hpp"

namespace internal_ {

namespace camera {

glm::vec3 GetFront(aderite::scene::Camera* camera) {
    return camera->getForwardDirection();
}

void linkCamera() {
    mono_add_internal_call("Aderite.Camera::__GetFront(intptr)", reinterpret_cast<void*>(GetFront));
}
} // namespace camera

namespace gameObject {
MonoString* GetName(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->string(gObject->getName().c_str());
}

MonoObject* GetTransform(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->getLocator().create(gObject->getTransform());
}

MonoObject* GetCamera(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->getLocator().create(gObject->getCamera());
}

MonoObject* GetBehavior(aderite::scene::GameObject* gObject, MonoObject* name) {
    std::string name_ = aderite::scripting::toString(name);

    for (aderite::scripting::ScriptedBehavior* behavior : gObject->getBehaviors()) {
        if (behavior->getBase()->getName() == name_) {
            return behavior->getInstance();
        }
    }

    return nullptr;
}

void Destroy(aderite::scene::GameObject* gObject) {
    gObject->markForDeletion();
}

void linkGameObject() {
    mono_add_internal_call("Aderite.GameObject::__GetName(intptr)", reinterpret_cast<void*>(GetName));
    mono_add_internal_call("Aderite.GameObject::__GetTransform(intptr)", reinterpret_cast<void*>(GetTransform));
    mono_add_internal_call("Aderite.GameObject::__GetCamera(intptr)", reinterpret_cast<void*>(GetCamera));
    mono_add_internal_call("Aderite.GameObject::__GetBehavior(intptr,string)", reinterpret_cast<void*>(GetBehavior));
    mono_add_internal_call("Aderite.GameObject::__Destroy(intptr)", reinterpret_cast<void*>(Destroy));
}
} // namespace gameObject

namespace transform {
glm::vec3 GetPosition(aderite::scene::TransformProvider* transform) {
    return transform->getPosition();
}

void SetPosition(aderite::scene::TransformProvider* transform, glm::vec3 position) {
    transform->setPosition(position);
}

glm::quat GetRotation(aderite::scene::TransformProvider* transform) {
    return transform->getRotation();
}

void SetRotation(aderite::scene::TransformProvider* transform, glm::quat rotation) {
    transform->setRotation(rotation);
}

void linkTransform() {
    mono_add_internal_call("Aderite.Transform::__GetPosition(intptr)", reinterpret_cast<void*>(GetPosition));
    mono_add_internal_call("Aderite.Transform::__SetPosition(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetPosition));
    mono_add_internal_call("Aderite.Transform::__GetRotation(intptr)", reinterpret_cast<void*>(GetRotation));
    mono_add_internal_call("Aderite.Transform::__SetRotation(intptr,Aderite.Quaternion)", reinterpret_cast<void*>(SetRotation));
}
} // namespace transform

} // namespace internal_

void linkScene() {
    // Camera
    internal_::camera::linkCamera();

    // GameObject
    internal_::gameObject::linkGameObject();

    // Transform
    internal_::transform::linkTransform();
}
