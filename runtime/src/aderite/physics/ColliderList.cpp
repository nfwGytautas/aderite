#include "ColliderList.hpp"

#include <PxRigidDynamic.h>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"

namespace aderite {
namespace physics {

ColliderList::~ColliderList() {
	for (Collider* c : m_colliders) {
		delete c;
	}

	if (m_actor) {
		m_actor->release();
		m_actor = nullptr;
	}
}

ColliderList::ColliderList() {}

void ColliderList::addCollider(Collider* collider) {
	if (m_actor != nullptr) {
		collider->setActor(m_actor);
	}

	m_colliders.push_back(collider);
}

void ColliderList::removeCollider(Collider* collider) {
	auto& it = std::find(m_colliders.begin(), m_colliders.end(), collider);

	if (it != m_colliders.end()) {
		delete* it;
		m_colliders.erase(it);
	}
}

void ColliderList::setScale(const glm::vec3& scale) {
	for (Collider* c : m_colliders) {
		c->setScale(scale);
	}
}

void ColliderList::setActor(physx::PxRigidActor* actor) {
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

reflection::Type aderite::physics::ColliderList::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::CLDR_LIST);
}

bool aderite::physics::ColliderList::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Colliders";
	emitter << YAML::BeginSeq;
	for (Collider* c : m_colliders) {
		serializer->writeUntrackedType(emitter, c);
	}
	emitter << YAML::EndSeq;
	return true;
}

bool aderite::physics::ColliderList::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	for (auto colliderEntry : data["Colliders"]) {
		YAML::Node& colliderNode = colliderEntry;
		Collider* collider = static_cast<Collider*>(serializer->parseUntrackedType(data));
		addCollider(collider);
	}

	return true;
}

}
}
