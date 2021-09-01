#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <PxShape.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief Collider base class
*/
class Collider : public interfaces::ISerializable {
public:
	/**
	 * @brief Creates a collider from geometry
	 * @param geometry Geometry of the collider (will be deleted)
	*/
	Collider(physx::PxGeometry* geometry);

	virtual ~Collider();

	/**
	 * @brief Returns the type of the collider
	*/
	virtual ColliderType getType() const = 0;

	/**
	 * @brief Returns true if the collider acts as a trigger or not
	*/
	bool isTrigger() const;

	/**
	 * @brief Converts the collider to a trigger if true, if false converts to collider
	*/
	void setTrigger(bool value);

	/**
	 * @brief Sets the physics material for the collider
	*/
	void setMaterial(asset::Asset* material);

	/**
	 * @brief Sets the actor of the collider, this will remove the collider from previous actor
	 * @param actor New actor of the collider
	*/
	void setActor(physx::PxRigidActor* actor);

	/**
	 * @brief Returns the physics material of the collider, nullptr if not set
	*/
	asset::Asset* getMaterial() const;

	/**
	 * @brief Sets the scale of the collider
	 * @param scale New scale of the collider
	*/
	virtual void setScale(const glm::vec3& scale) = 0;

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

protected:
	// Not serialized
	physx::PxShape* p_shape = nullptr;

private:
	asset::Asset* m_material = nullptr;
};

ADERITE_PHYSICS_NAMESPACE_END
