#include "ScriptPhysics.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <mono/jit/jit.h>

#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/StaticActor.hpp"

namespace aderite {
namespace scripting {

void MoveActor(physics::PhysicsActor* actor, glm::vec3 position) {
    actor->moveActor(position);
}

void RotateActor(physics::PhysicsActor* actor, glm::quat rotation) {
    actor->rotateActor(rotation);
}

bool GetActorKinematic(physics::DynamicActor* actor) {
    return actor->getKinematic();
}

bool GetActorGravity(physics::DynamicActor* actor) {
    return actor->getGravity();
}

float GetActorMass(physics::DynamicActor* actor) {
    return actor->getMass();
}

void SetActorKinematic(physics::DynamicActor* actor, bool value) {
    actor->setKinematic(value);
}

void SetActorGravity(physics::DynamicActor* actor, bool value) {
    actor->setGravity(value);
}

void SetActorMass(physics::DynamicActor* actor, float value) {
    actor->setMass(value);
}

void physicsInternals() {
    // Base actor
    mono_add_internal_call("Aderite.PhysicsActor::__Move(intptr,Aderite.Vector3)", reinterpret_cast<void*>(MoveActor));
    mono_add_internal_call("Aderite.PhysicsActor::__Rotate(intptr,Aderite.Quaternion)", reinterpret_cast<void*>(RotateActor));

    // Dynamic
    mono_add_internal_call("Aderite.DynamicActor::__GetKinematic(intptr)", reinterpret_cast<void*>(GetActorKinematic));
    mono_add_internal_call("Aderite.DynamicActor::__GetGravity(intptr)", reinterpret_cast<void*>(GetActorGravity));
    mono_add_internal_call("Aderite.DynamicActor::__GetMass(intptr)", reinterpret_cast<void*>(GetActorMass));

    mono_add_internal_call("Aderite.DynamicActor::__SetKinematic(intptr,bool)", reinterpret_cast<void*>(SetActorKinematic));
    mono_add_internal_call("Aderite.DynamicActor::__SetGravity(intptr,bool)", reinterpret_cast<void*>(SetActorGravity));
    mono_add_internal_call("Aderite.DynamicActor::__SetMass(intptr,single)", reinterpret_cast<void*>(SetActorMass));

    // Static
}

} // namespace scripting
} // namespace aderite
