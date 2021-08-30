#include "BoxCollider.hpp"

#include <PxPhysics.h>
#include <PxMaterial.h>

#include "aderite/utility/YAML.hpp"

ADERITE_COLLIDER_NAMESPACE_BEGIN

glm::vec3 BoxCollider::getSize() const {
	return m_size;
}

void BoxCollider::setSize(const glm::vec3 size) {
	m_size = size;
}

bool BoxCollider::serialize(YAML::Emitter& out) {
	// TODO: Error check
	Collider::serialize(out);
	out << YAML::Key << "Size" << YAML::Value << m_size;
	return true;
}

bool BoxCollider::deserialize(YAML::Node& data) {
	Collider::deserialize(data);
	m_size = data["Size"].as<glm::vec3>();
	return true;
}

ColliderType BoxCollider::getType() const {
	return ColliderType::BOX;
}

physx::PxShape* BoxCollider::construct(physx::PxPhysics* physics, physx::PxMaterial* material) {
	physx::PxShape* shape = physics->createShape(
		physx::PxBoxGeometry(m_size.x, m_size.y, m_size.z),
		*material,
		true);

	return shape;
}

ADERITE_COLLIDER_NAMESPACE_END
