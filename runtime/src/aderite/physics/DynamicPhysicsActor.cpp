#include "DynamicPhysicsActor.hpp"

#include <physx/PxRigidDynamic.h>
#include <physx/PxScene.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"

namespace aderite {
namespace physics {

DynamicPhysicsActor::DynamicPhysicsActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
    p_actor->userData = this;
}

DynamicPhysicsActor::~DynamicPhysicsActor() {}

void DynamicPhysicsActor::setGravity(bool value) {
    p_actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}

bool DynamicPhysicsActor::getGravity() const {
    return !(static_cast<physx::PxRigidDynamic*>(p_actor)->getActorFlags() & physx::PxActorFlag::eDISABLE_GRAVITY);
}

void DynamicPhysicsActor::setMass(float mass) {
    static_cast<physx::PxRigidDynamic*>(p_actor)->setMass(mass);
}

float DynamicPhysicsActor::getMass() const {
    return static_cast<float>(static_cast<physx::PxRigidDynamic*>(p_actor)->getMass());
}

bool DynamicPhysicsActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "DynamicPhysicsActor" << YAML::BeginMap;
    if (!BasePhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    // Properties
    emitter << YAML::Key << "Mass" << YAML::Value << this->getMass();
    emitter << YAML::Key << "HasGravity" << YAML::Value << this->getGravity();

    emitter << YAML::EndMap;

    return true;
}

bool DynamicPhysicsActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& actorNode = data["DynamicPhysicsActor"];
    if (!actorNode || actorNode.IsNull()) {
        return false;
    }

    if (!BasePhysicsActor::deserialize(serializer, actorNode)) {
        return false;
    }

    // Properties
    this->setMass(actorNode["Mass"].as<float>());
    this->setGravity(actorNode["HasGravity"].as<bool>());

    return true;
}

} // namespace physics
} // namespace aderite
