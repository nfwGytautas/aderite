#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <PxShape.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief A list of colliders used for an entity
*/
class ColliderList : public interfaces::ISerializable {
public:
	~ColliderList();

	/**
	 * @brief Adds a box colider to the list
	 * @param halfExtents Half extents of the collider
	*/
	void addBoxCollider(const glm::vec3& halfExtents);

	/**
	 * @brief Assigns the collider list to the specified rigid body
	 * @param rbody Rigidbody to assign to
	*/
	void assignToRigidbody(Rigidbody* rbody);

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
private:
	ColliderList() {}
	friend PhysicsController;

	std::vector<physx::PxShape*> m_colliders;
};

ADERITE_PHYSICS_NAMESPACE_END
