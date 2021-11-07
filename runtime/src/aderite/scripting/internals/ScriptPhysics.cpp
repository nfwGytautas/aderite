#include "ScriptPhysics.hpp"

#include <mono/jit/jit.h>

// TEMP
#include <entt/entt.hpp>

#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void MoveActor(scene::Scene* scene, entt::entity entity, glm::vec3 position) {
    //// Can be either static or dynamic actor
    // scene::Entity e = scene::Entity(entity, scene);
    //
    // if (e.hasComponent<scene::DynamicActor>()) {
    //    e.getComponent<scene::DynamicActor>().Actor->moveActor(position);
    //} else {
    //    // Should be static actor
    //    e.getComponent<scene::StaticActor>().Actor->moveActor(position);
    //}
}

void RotateActor(scene::Scene* scene, entt::entity entity, glm::quat rotation) {
    //// Can be either static or dynamic actor
    // scene::Entity e = scene::Entity(entity, scene);

    // if (e.hasComponent<scene::DynamicActor>()) {
    //    e.getComponent<scene::DynamicActor>().Actor->rotateActor(rotation);
    //} else {
    //    // Should be static actor
    //    e.getComponent<scene::StaticActor>().Actor->rotateActor(rotation);
    //}
}

bool GetActorKinematic(scene::Scene* scene, entt::entity entity) {
    // return scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->getKinematic();
    return true;
}

bool GetActorGravity(scene::Scene* scene, entt::entity entity) {
    // return scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->getGravity();
    return true;
}

float GetActorMass(scene::Scene* scene, entt::entity entity) {
    // return scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->getMass();
    return 0.0f;
}

void SetActorKinematic(scene::Scene* scene, entt::entity entity, bool value) {
    // scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->setKinematic(value);
}

void SetActorGravity(scene::Scene* scene, entt::entity entity, bool value) {
    // scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->setGravity(value);
}

void SetActorMass(scene::Scene* scene, entt::entity entity, float value) {
    // scene::Entity(entity, scene).getComponent<scene::DynamicActor>().Actor->setMass(value);
}

void physicsInternals() {
    // Base actor
    mono_add_internal_call("Aderite.PhysicsActor::__Move(intptr,uintptr,Aderite.Vector3)", reinterpret_cast<void*>(MoveActor));
    mono_add_internal_call("Aderite.PhysicsActor::__Rotate(intptr,uintptr,Aderite.Quaternion)", reinterpret_cast<void*>(RotateActor));

    // Dynamic
    mono_add_internal_call("Aderite.DynamicActor::__GetKinematic(intptr,uintptr)", reinterpret_cast<void*>(GetActorKinematic));
    mono_add_internal_call("Aderite.DynamicActor::__GetGravity(intptr,uintptr)", reinterpret_cast<void*>(GetActorGravity));
    mono_add_internal_call("Aderite.DynamicActor::__GetMass(intptr,uintptr)", reinterpret_cast<void*>(GetActorMass));

    mono_add_internal_call("Aderite.DynamicActor::__SetKinematic(intptr,uintptr,bool)", reinterpret_cast<void*>(SetActorKinematic));
    mono_add_internal_call("Aderite.DynamicActor::__SetGravity(intptr,uintptr,bool)", reinterpret_cast<void*>(SetActorGravity));
    mono_add_internal_call("Aderite.DynamicActor::__SetMass(intptr,uintptr,single)", reinterpret_cast<void*>(SetActorMass));

    // Static
}

} // namespace scripting
} // namespace aderite
