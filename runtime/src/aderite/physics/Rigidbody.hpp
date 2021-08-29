#pragma once

#include <vector>
#include <PxPhysics.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/ISerializable.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief Wrapper for PhysX dynamic rigid bodies and actors
*/
class Rigidbody : public interfaces::ISerializable {
public:
	~Rigidbody();

	/**
	 * @brief Update entity transform with the state of the rigid body
	 * @param delta Delta time between two frames
	*/
	void update(float delta);

	/**
	 * @brief Disables or enables gravity for the rigid body depending on the passed value
	 * @param value If true enables gravity, otherwise disables it
	*/
	void setGravity(bool value);

	/**
	 * @brief Returns true if the rigid body is affected by gravity, false otherwise
	*/
	bool hasGravity();

	/**
	 * @brief Sets the object mass, 0.0f meaning infinite
	*/
	void setMass(float mass);

	/**
	 * @brief Returns the mass of the rigid body
	*/
	float getMass();

	/**
	 * @brief Returns the underlying PhysX handle
	*/
	physx::PxRigidDynamic* getHandle();

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
private:
	Rigidbody(PhysicsController* pc, scene::Entity on);
	friend PhysicsController;

	physx::PxRigidDynamic* m_rbody = nullptr;
	scene::Entity m_on = scene::Entity::null();
};

ADERITE_PHYSICS_NAMESPACE_END
