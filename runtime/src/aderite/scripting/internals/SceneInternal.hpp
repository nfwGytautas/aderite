#pragma once

#ifndef ADERITE_SCENE_INTERNALS
#define ADERITE_SCENE_INTERNALS
#else
#error "Multiple SceneInternal.hpp include"
#endif

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace internal_ {

namespace camera {
void linkCamera() {}
} // namespace camera

namespace gameObject {
MonoString* GetName(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->string(gObject->getName().c_str());
}

MonoObject* GetTransform(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->getLocator().create(gObject->getTransform());
}

void Destroy(aderite::scene::GameObject* gObject) {
    gObject->markForDeletion();
}

void linkGameObject() {
    mono_add_internal_call("Aderite.GameObject::__GetName(intptr)", reinterpret_cast<void*>(GetName));
    mono_add_internal_call("Aderite.GameObject::__GetTransform(intptr)", reinterpret_cast<void*>(GetTransform));
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

void linkTransform() {
    mono_add_internal_call("Aderite.Transform::__GetPosition(intptr)", reinterpret_cast<void*>(GetPosition));
    mono_add_internal_call("Aderite.Transform::__SetPosition(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetPosition));
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
