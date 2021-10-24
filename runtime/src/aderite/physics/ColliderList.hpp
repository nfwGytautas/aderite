#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <PxShape.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A list of colliders used for an entity
*/
class ColliderList : public io::ISerializable {
public:
	ColliderList();
	~ColliderList();

	/**
	 * @brief Add an already created collider, this collider is managed by the collider list
	 * @param collider Collider to add
	*/
	void addCollider(Collider* collider);

	/**
	 * @brief Removes collider from list
	 * @param collider Collider to remove
	*/
	void removeCollider(Collider* collider);

	/**
	 * @brief Sets the scale of all colliders in the list
	 * @param scale New scale of the colliders
	*/
	void setScale(const glm::vec3& scale);

	/**
	 * @brief Applies colliders in the list to the actor
	 * @param actor Actor to apply to
	*/
	void setActor(physx::PxRigidActor* actor);

	/**
	 * @brief Returns the actor of colliders
	*/
	physx::PxRigidActor* getActor() const;

	Collider* get(size_t idx) const {
		return m_colliders[idx];
	}

	Collider* operator[](size_t idx) const {
		return m_colliders[idx];
	}

	size_t size() const {
		return m_colliders.size();
	}

	auto begin() const {
		return m_colliders.begin();
	}

	auto end() const {
		return m_colliders.end();
	}

	// Inherited via ISerializable
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::vector<Collider*> m_colliders;

	// Runtime variable set by physics controller
	physx::PxRigidActor* m_actor = nullptr;
};

}
}
