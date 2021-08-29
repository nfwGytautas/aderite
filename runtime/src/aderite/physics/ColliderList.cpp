#include "ColliderList.hpp"

#include <PxRigidDynamic.h>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/physics/Rigidbody.hpp"
#include "aderite/physics/PhysicsController.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

ColliderList::~ColliderList() {
	for (physx::PxShape* c : m_colliders) {
		c->release();
	}
}

void ColliderList::addBoxCollider(const glm::vec3& halfExtents) {
	physx::PxShape* shape = ::aderite::Engine::getPhysicsController()->getPhysics()->createShape(
		physx::PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z),
		*::aderite::Engine::getPhysicsController()->getDefaultMaterial(),
		true);

	m_colliders.push_back(shape);
}

void ColliderList::assignToRigidbody(Rigidbody* rbody) {
	for (physx::PxShape* c : m_colliders) {
		rbody->getHandle()->attachShape(*c);
	}
}

bool ColliderList::serialize(YAML::Emitter& out) {
	for (physx::PxShape* c : m_colliders) {
		out << YAML::BeginMap;

		out << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(c->getGeometryType());

		switch (c->getGeometryType()) {
		case physx::PxGeometryType::eBOX: {
			physx::PxBoxGeometry box;
			if (!c->getBoxGeometry(box)) {
				LOG_ERROR("Failed to get geometry of collider, aborting");
				return false;
			}

			glm::vec3 v = { box.halfExtents.x, box.halfExtents.y, box.halfExtents.z };
			out << YAML::Key << "HalfExtents" << YAML::Value << v;
			break;
		}
		default: {
			LOG_ERROR("Unsupported collider, aborting");
			return false;
		}
		}

		out << YAML::EndMap;
	}

	return true;
}

bool ColliderList::deserialize(YAML::Node& data) {
	if (!data.IsSequence()) {
		return false;
	}

	for (auto colliderEntry : data) {
		YAML::Node& colliderNode = colliderEntry;
		physx::PxGeometryType::Enum type = (physx::PxGeometryType::Enum)(colliderNode["Type"].as<size_t>());
		switch (type) {
		case physx::PxGeometryType::eBOX: {
			glm::vec3 halfExtents = colliderNode["HalfExtents"].as<glm::vec3>();
			addBoxCollider(halfExtents);
			break;
		}
		default: {
			LOG_ERROR("Unsupported collider, aborting");
			return false;
		}
		}
	}

	return true;
}

ADERITE_PHYSICS_NAMESPACE_END
