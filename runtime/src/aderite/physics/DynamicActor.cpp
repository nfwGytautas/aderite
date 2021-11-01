#include "DynamicActor.hpp"

#include <PxRigidDynamic.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"

namespace aderite {
namespace physics {

DynamicActor::DynamicActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->createDynamicBody();
    p_actor->userData = this;
}

void DynamicActor::isKinematic(bool value) {
    static_cast<physx::PxRigidDynamic*>(p_actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}

void DynamicActor::hasGravity(bool value) {
    p_actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, value);
}

void DynamicActor::setMass(float mass) {
    static_cast<physx::PxRigidDynamic*>(p_actor)->setMass(mass);
}

reflection::Type DynamicActor::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::DYNAMIC_ACTOR);
}

bool DynamicActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    return true;
}

bool DynamicActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    this->isKinematic(data["IsKinematic"].as<bool>());
    this->hasGravity(data["HasGravity"].as<bool>());
    this->setMass(data["Mass"].as<float>());
    return true;
}

} // namespace physics
} // namespace aderite
