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
	 * @brief Add an already created collider, this collider is managed by the collider list
	 * @param collider Collider to add
	*/
	void addCollider(Collider* collider);

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	auto begin() {
		return m_colliders.begin();
	}

	auto begin() const {
		return m_colliders.begin();
	}

	auto end() {
		return m_colliders.end();
	}

	auto end() const {
		return m_colliders.end();
	}
private:
	std::vector<Collider*> m_colliders;
};

ADERITE_PHYSICS_NAMESPACE_END
