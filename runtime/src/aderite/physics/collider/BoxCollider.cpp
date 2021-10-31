#include "BoxCollider.hpp"

#include <PxMaterial.h>
#include <PxPhysics.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

BoxCollider::BoxCollider() : Collider(new physx::PxBoxGeometry(1.0f, 1.0f, 1.0f)) {}

void BoxCollider::setScale(const glm::vec3& scale) {
    physx::PxBoxGeometry geom;
    p_shape->getBoxGeometry(geom);
    geom.halfExtents = {m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z};
    p_shape->setGeometry(geom);
}

glm::vec3 BoxCollider::getSize() const {
    return m_size;
}

void BoxCollider::setSize(const glm::vec3 size) {
    physx::PxBoxGeometry geom;
    p_shape->getBoxGeometry(geom);
    geom.halfExtents = {(geom.halfExtents.x / m_size.x) * size.x, (geom.halfExtents.y / m_size.y) * size.y,
                        (geom.halfExtents.z / m_size.z) * size.z};
    p_shape->setGeometry(geom);
    m_size = size;
}

reflection::Type BoxCollider::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::BOX_CLDR);
}

bool BoxCollider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    Collider::serialize(serializer, emitter);
    emitter << YAML::Key << "Size" << YAML::Value << m_size;
    return true;
}

bool BoxCollider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    Collider::deserialize(serializer, data);
    glm::vec3 size = data["Size"].as<glm::vec3>();
    setSize(size);
    return true;
}

} // namespace physics
} // namespace aderite
