#include "BoxGeometry.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

BoxGeometry::BoxGeometry() {
    // Create shape object
    physx::PxPhysics* physics = ::aderite::Engine::getPhysicsController()->getPhysics();
    physx::PxMaterial* defaultMaterial = ::aderite::Engine::getPhysicsController()->getDefaultMaterial();
    physx::PxShapeFlags baseFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                    physx::PxShapeFlag::eSIMULATION_SHAPE;

    // Create geometry and shape
    physx::PxGeometry* geometry = new physx::PxBoxGeometry(m_size.x, m_size.y, m_size.z);
    p_shape = physics->createShape(*geometry, *defaultMaterial, true, baseFlags);
    delete geometry;

    if (p_shape == nullptr) {
        LOG_ERROR("[Physics] Failed to create {0:p} collider shape", static_cast<void*>(this));
        return;
    }

    // Set user data
    p_shape->userData = this;
}

glm::vec3 BoxGeometry::getSize() const {
    return m_size;
}

void BoxGeometry::setSize(const glm::vec3 size) {
    physx::PxBoxGeometry geom;
    p_shape->getBoxGeometry(geom);
    geom.halfExtents = {(geom.halfExtents.x / m_size.x) * size.x, (geom.halfExtents.y / m_size.y) * size.y,
                        (geom.halfExtents.z / m_size.z) * size.z};
    p_shape->setGeometry(geom);
    m_size = size;
}

void BoxGeometry::applyScale(const glm::vec3& scale) {
    physx::PxBoxGeometry geom;
    p_shape->getBoxGeometry(geom);
    geom.halfExtents = {m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z};
    p_shape->setGeometry(geom);
}

bool BoxGeometry::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Size" << YAML::Value << m_size;
    emitter << YAML::Key << "IsTrigger" << YAML::Value << this->isTrigger();
    return true;
}

bool BoxGeometry::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    setSize(data["Size"].as<glm::vec3>());
    setTrigger(data["IsTrigger"].as<bool>());
    return true;
}

reflection::Type BoxGeometry::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::BOX_GEOMETRY);
}

Geometry* BoxGeometry::clone() {
    BoxGeometry* bg = new BoxGeometry();
    bg->setSize(bg->getSize());
    bg->setName(bg->getName());
    return bg;
}

} // namespace physics
} // namespace aderite
