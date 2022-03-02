#pragma once

#ifndef ADERITE_PHYSICS_INTERNALS
#define ADERITE_PHYSICS_INTERNALS
#else
#error "Multiple PhysicsInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"

namespace internal_ {

namespace physics {

MonoObject* RaycastSingle(glm::vec3 from, glm::vec3 direction, float distance) {
    aderite::scene::Scene* scene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (scene == nullptr) {
        LOG_ERROR("[Scripting] Tried to call Aderite.Physics::__RaycastSingle when no active scene exists");
        return nullptr;
    }

    // Do a raycast
    aderite::physics::RaycastResult hit;
    if (scene->raycastSingle(hit, from, direction, distance)) {
        return ::aderite::Engine::getScriptManager()->getLocator().create(hit);
    }

    // No hit
    return nullptr;
}

void linkPhysics() {
    mono_add_internal_call("Aderite.Physics::__RaycastSingle(Aderite.Vector3,Aderite.Vector3,single)",
                           reinterpret_cast<void*>(RaycastSingle));
}
} // namespace physics

} // namespace internal_

void linkPhysics() {
    // Physics
    internal_::physics::linkPhysics();
}
