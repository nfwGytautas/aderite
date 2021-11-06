#include "BoxCollider.hpp"

#include <PxMaterial.h>
#include <PxPhysics.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

glm::vec3 BoxCollider::getSize() const {
    return m_size;
}

void BoxCollider::setSize(const glm::vec3 size) {
    m_size = size;
}

reflection::Type BoxCollider::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::BOX_CLDR);
}

bool BoxCollider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
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

physx::PxGeometry* BoxCollider::genGeometry(const glm::vec3& globalScale) {
    return new physx::PxBoxGeometry(m_size.x * globalScale.x, m_size.y * globalScale.y, m_size.z * globalScale.z);
}

} // namespace physics
} // namespace aderite
