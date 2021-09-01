#include "ColliderList.hpp"

#include <PxRigidDynamic.h>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

ColliderList::~ColliderList() {
	for (Collider* c : m_colliders) {
		delete c;
	}

	m_actor->release();
	m_actor = nullptr;
}

ColliderList::ColliderList(scene::Entity entity) 
	: m_entity(entity)
{}

void ColliderList::addCollider(Collider* collider) {
	if (m_actor != nullptr) {
		collider->setActor(m_actor);
	}

	m_colliders.push_back(collider);
}

void ColliderList::removeCollider(Collider* collider) {
	auto& it = std::find(m_colliders.begin(), m_colliders.end(), collider);

	if (it != m_colliders.end()) {
		delete *it;
		m_colliders.erase(it);
	}
}

void ColliderList::setScale(const glm::vec3& scale) {
	for (Collider* c : m_colliders) {
		c->setScale(scale);
	}
}

void ColliderList::setActor(physx::PxRigidActor* actor) {
	actor->userData = &m_entity;

	for (Collider* c : m_colliders) {
		c->setActor(actor);
	}

	// Release old actor
	if (m_actor != nullptr) {
		m_actor->release();
	}

	m_actor = actor;
}

physx::PxRigidActor* ColliderList::getActor() const {
	return m_actor;
}

bool ColliderList::serialize(YAML::Emitter& out) {
	for (Collider* c : m_colliders) {
		out << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(c->getType());
		c->serialize(out);
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

		// Resolve type
		ColliderType type = static_cast<physics::ColliderType>((colliderNode["Type"].as<size_t>()));
		Collider* collider = nullptr;
		switch (type) {
		case ColliderType::BOX: {
			collider = new collider::BoxCollider();
			break;
		}
		default: {
			LOG_ERROR("Unsupported collider, aborting");
			return false;
		}
		}

		// Load it
		collider->deserialize(colliderNode);
		addCollider(collider);
	}

	return true;
}

ADERITE_PHYSICS_NAMESPACE_END
