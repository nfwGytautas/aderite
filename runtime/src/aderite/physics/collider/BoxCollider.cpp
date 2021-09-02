#include "BoxCollider.hpp"

#include <PxPhysics.h>
#include <PxMaterial.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/physics/PhysicsController.hpp"

ADERITE_COLLIDER_NAMESPACE_BEGIN

BoxCollider::BoxCollider()
	: Collider(new physx::PxBoxGeometry(1.0f, 1.0f, 1.0f))
{}

void BoxCollider::setScale(const glm::vec3& scale) {
	physx::PxBoxGeometry geom;
	p_shape->getBoxGeometry(geom);
	geom.halfExtents = { 
		m_size.x * scale.x, 
		m_size.y * scale.y, 
		m_size.z * scale.z 
	};
	p_shape->setGeometry(geom);
}

glm::vec3 BoxCollider::getSize() const {
	return m_size;
}

void BoxCollider::setSize(const glm::vec3 size) {
	physx::PxBoxGeometry geom;
	p_shape->getBoxGeometry(geom);
	geom.halfExtents = { 
		(geom.halfExtents.x / m_size.x) * size.x, 
		(geom.halfExtents.y / m_size.y) * size.y, 
		(geom.halfExtents.z / m_size.z) * size.z 
	};
	p_shape->setGeometry(geom);
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
	glm::vec3 size = data["Size"].as<glm::vec3>();
	setSize(size);
	return true;
}

ColliderType BoxCollider::getType() const {
	return ColliderType::BOX;
}

ADERITE_COLLIDER_NAMESPACE_END
