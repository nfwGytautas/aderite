#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <PxShape.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"
#include "aderite/scene/Entity.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief A list of colliders used for an entity
*/
class ColliderList : public interfaces::ISerializable {
public:
	ColliderList(scene::Entity entity);
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

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	auto begin() const {
		return m_colliders.begin();
	}

	auto end() const {
		return m_colliders.end();
	}
private:
	scene::Entity m_entity = scene::Entity::null();
	std::vector<Collider*> m_colliders;

	// Runtime variable set by physics controller
	physx::PxRigidActor* m_actor = nullptr;
};

ADERITE_PHYSICS_NAMESPACE_END
