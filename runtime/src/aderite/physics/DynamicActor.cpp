#include "DynamicActor.hpp"

#include <PxRigidDynamic.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace physics {

DynamicActor::DynamicActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
    p_actor->userData = this;
}

bool DynamicActor::getKinematic() const {
    return (static_cast<physx::PxRigidDynamic*>(p_actor)->getRigidBodyFlags() & physx::PxRigidBodyFlag::eKINEMATIC);
}

void DynamicActor::setKinematic(bool value) {
    LOG_TRACE("[Physics] Setting actor {0:p} kinematic flag to {1}", static_cast<void*>(this), value);
    static_cast<physx::PxRigidDynamic*>(p_actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}

void DynamicActor::setGravity(bool value) {
    LOG_TRACE("[Physics] Setting actor {0:p} gravity flag to {1}", static_cast<void*>(this), value);
    p_actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}

bool DynamicActor::getGravity() const {
    return !(static_cast<physx::PxRigidDynamic*>(p_actor)->getActorFlags() & physx::PxActorFlag::eDISABLE_GRAVITY);
}

void DynamicActor::setMass(float mass) {
    LOG_TRACE("[Physics] Setting actor {0:p} mass to {1}", static_cast<void*>(this), mass);
    static_cast<physx::PxRigidDynamic*>(p_actor)->setMass(mass);
}

float DynamicActor::getMass() const {
    return static_cast<float>(static_cast<physx::PxRigidDynamic*>(p_actor)->getMass());
}

reflection::Type DynamicActor::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::DYNAMIC_ACTOR);
}

bool DynamicActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!PhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "IsKinematic" << YAML::Value << this->getKinematic();
    emitter << YAML::Key << "HasGravity" << YAML::Value << this->getGravity();
    emitter << YAML::Key << "Mass" << YAML::Value << this->getMass();
    return true;
}

bool DynamicActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!PhysicsActor::deserialize(serializer, data)) {
        return false;
    }

    this->setKinematic(data["IsKinematic"].as<bool>());
    this->setGravity(data["HasGravity"].as<bool>());
    this->setMass(data["Mass"].as<float>());
    return true;
}

PhysicsActor* DynamicActor::clone() const {
    DynamicActor* actor = new DynamicActor();

    // Properties
    actor->setKinematic(this->getKinematic());
    actor->setGravity(this->getGravity());
    actor->setMass(this->getMass());

    // Colliders
    this->cloneInto(actor);
    return actor;
}

} // namespace physics
} // namespace aderite
