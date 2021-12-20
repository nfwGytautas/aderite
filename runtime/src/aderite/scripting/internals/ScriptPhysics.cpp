#include "ScriptPhysics.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/LibClassLocator.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

MonoObject* geometryGetEntity(physics::Geometry* geometry) {
    return ::aderite::Engine::getScriptManager()->createInstance(static_cast<scene::Entity*>(geometry->getActor()));
}

// void MoveActor(physics::PhysicsActor* actor, glm::vec3 position) {
//    actor->moveActor(position);
//}
//
// void RotateActor(physics::PhysicsActor* actor, glm::quat rotation) {
//    actor->rotateActor(rotation);
//}
//
// bool GetActorKinematic(physics::DynamicActor* actor) {
//    return actor->getKinematic();
//}
//
// bool GetActorGravity(physics::DynamicActor* actor) {
//    return actor->getGravity();
//}
//
// float GetActorMass(physics::DynamicActor* actor) {
//    return actor->getMass();
//}
//
// void SetActorKinematic(physics::DynamicActor* actor, bool value) {
//    actor->setKinematic(value);
//}
//
// void SetActorGravity(physics::DynamicActor* actor, bool value) {
//    actor->setGravity(value);
//}
//
// void SetActorMass(physics::DynamicActor* actor, float value) {
//    actor->setMass(value);
//}
//
// MonoObject* RaycastSingle(glm::vec3 from, glm::vec3 direction, float distance) {
//    scene::Scene* scene = ::aderite::Engine::getSceneManager()->getCurrentScene();
//    if (scene == nullptr) {
//        LOG_ERROR("[Scripting] Tried to call Aderite.Physics::__RaycastSingle when no active scene exists");
//        return nullptr;
//    }
//
//    // Do a raycast
//    physics::RaycastHit hit;
//    if (scene->getPhysicsScene()->raycastSingle(hit, from, direction, distance)) {
//        return ::aderite::Engine::getScriptManager()->getLocator().create(hit);
//    }
//
//    // No hit
//    return nullptr;
//}

void physicsInternals() {
    // Geometry
    mono_add_internal_call("Aderite.Geometry::__GetEntity(intptr)", reinterpret_cast<void*>(geometryGetEntity));

    //// Base actor
    // mono_add_internal_call("Aderite.PhysicsActor::__Move(intptr,Aderite.Vector3)", reinterpret_cast<void*>(MoveActor));
    // mono_add_internal_call("Aderite.PhysicsActor::__Rotate(intptr,Aderite.Quaternion)", reinterpret_cast<void*>(RotateActor));

    //// Dynamic
    // mono_add_internal_call("Aderite.DynamicActor::__GetKinematic(intptr)", reinterpret_cast<void*>(GetActorKinematic));
    // mono_add_internal_call("Aderite.DynamicActor::__GetGravity(intptr)", reinterpret_cast<void*>(GetActorGravity));
    // mono_add_internal_call("Aderite.DynamicActor::__GetMass(intptr)", reinterpret_cast<void*>(GetActorMass));

    // mono_add_internal_call("Aderite.DynamicActor::__SetKinematic(intptr,bool)", reinterpret_cast<void*>(SetActorKinematic));
    // mono_add_internal_call("Aderite.DynamicActor::__SetGravity(intptr,bool)", reinterpret_cast<void*>(SetActorGravity));
    // mono_add_internal_call("Aderite.DynamicActor::__SetMass(intptr,single)", reinterpret_cast<void*>(SetActorMass));

    //// Static

    //// Physics
    // mono_add_internal_call("Aderite.Physics::__RaycastSingle(Aderite.Vector3,Aderite.Vector3,single)",
    //                       reinterpret_cast<void*>(RaycastSingle));
}

} // namespace scripting
} // namespace aderite
