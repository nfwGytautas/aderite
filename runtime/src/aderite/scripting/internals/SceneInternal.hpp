#pragma once

#ifndef ADERITE_SCENE_INTERNALS
#define ADERITE_SCENE_INTERNALS
#else
#error "Multiple SceneInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scene/GameObject.hpp"

namespace internal_ {

namespace camera {
void linkCamera() {}
} // namespace camera

namespace gameObject {
MonoString* GetName(aderite::scene::GameObject* gObject) {
    return ::aderite::Engine::getScriptManager()->string(gObject->getName().c_str());
}

void linkGameObject() {
    mono_add_internal_call("Aderite.GameObject::__GetName(intptr)", reinterpret_cast<void*>(GetName));
}
} // namespace gameObject

namespace transform {
void linkTransform() {}
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
