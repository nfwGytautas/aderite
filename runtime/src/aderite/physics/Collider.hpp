#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <PxShape.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief Collider base class
*/
class Collider : public interfaces::ISerializable {
public:
	virtual ~Collider() {}

	/**
	 * @brief Returns the type of the collider
	*/
	virtual ColliderType getType() const = 0;

	/**
	 * @brief Returns true if the collider acts as a trigger or not
	*/
	bool isTrigger() const;

	/**
	 * @brief Create a PhysX shape from collider
	 * @param physics PxPhysics instance
	 * @param material PxMaterial instance
	 * @return Created PhysX shape
	*/
	virtual physx::PxShape* construct(physx::PxPhysics* physics, physx::PxMaterial* material) = 0;

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
protected:
	bool p_isTrigger = false;
};

ADERITE_PHYSICS_NAMESPACE_END
