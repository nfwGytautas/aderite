#include "BoxCollider.hpp"

#include <PxMaterial.h>
#include <PxPhysics.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

glm::vec3 BoxCollider::getSize() const {
    return m_size;
}

void BoxCollider::setSize(const glm::vec3 size) {
    m_size = size;
    this->updateGeometry();
}

reflection::Type BoxCollider::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::BOX_CLDR);
}

bool BoxCollider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!Collider::serialize(serializer, emitter)) {
        return false;
    }
    emitter << YAML::Key << "Size" << YAML::Value << m_size;
    return true;
}

bool BoxCollider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!Collider::deserialize(serializer, data)) {
        return false;
    }
    setSize(data["Size"].as<glm::vec3>());
    return true;
}

physx::PxGeometry* BoxCollider::genGeometry() const {
    return new physx::PxBoxGeometry((m_size.x * p_scale.x), (m_size.y * p_scale.y), (m_size.z * p_scale.z));
}

} // namespace physics
} // namespace aderite
