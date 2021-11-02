#include "Collider.hpp"

#include <PxRigidActor.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

bool Collider::isTrigger() const {
    return m_isTrigger;
}

void Collider::setTrigger(bool value) {
    m_isTrigger = value;
}

void Collider::attach(PhysicsActor* actor, const glm::vec3& globalScale) {
    actor->p_actor->attachShape(*this->createShape(globalScale));
}

bool Collider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "IsTrigger" << YAML::Value << isTrigger();
    return true;
}

bool Collider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    setTrigger(data["IsTrigger"].as<bool>());
    return true;
}

physx::PxShape* Collider::createShape(const glm::vec3& globalScale) {
    physx::PxGeometry* geometry = this->genGeometry(globalScale);
    physx::PxPhysics* physics = ::aderite::Engine::getPhysicsController()->getPhysics();
    physx::PxMaterial* defaultMaterial = ::aderite::Engine::getPhysicsController()->getDefaultMaterial();
    physx::PxShapeFlags baseFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE;

    if (m_isTrigger) {
        baseFlags |= physx::PxShapeFlag::eTRIGGER_SHAPE;
    } else {
        baseFlags |= physx::PxShapeFlag::eSIMULATION_SHAPE;
    }

    physx::PxShape* shape = physics->createShape(*geometry, *defaultMaterial, true, baseFlags);
    delete geometry;

    if (shape == nullptr) {
        LOG_ERROR("Failed to create collider shape");
        return nullptr;
    }

    return shape;
}

} // namespace physics
} // namespace aderite
