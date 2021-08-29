#include "Rigidbody.hpp"

#include <PxRigidBody.h>
#include <PxRigidDynamic.h>

#include "aderite/utility/Log.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/scene/components/Components.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

Rigidbody::~Rigidbody() {
	m_rbody->release();
}

void Rigidbody::update(float delta) {
	// TODO: Rotation
	physx::PxTransform t = m_rbody->getGlobalPose();
	auto& transform = m_on.getComponent<scene::components::TransformComponent>();
	transform.Position = { t.p.x, t.p.y, t.p.z };
}

Rigidbody::Rigidbody(PhysicsController* pc, scene::Entity on)
	: m_on(on)
{
	auto& transform = on.getComponent<scene::components::TransformComponent>();
	auto& rbodyComponent = on.getComponent<scene::components::RigidbodyComponent>();
	m_rbody = pc->getPhysics()->createRigidDynamic(
		physx::PxTransform(physx::PxVec3{ transform.Position.x, transform.Position.y, transform.Position.z }));
	m_rbody->userData = this;
	rbodyComponent.Body = this;
}

void Rigidbody::setGravity(bool value) {
	m_rbody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}

bool Rigidbody::hasGravity() {
	auto flags = m_rbody->getActorFlags();
	return !(flags & physx::PxActorFlag::eDISABLE_GRAVITY);
}

void Rigidbody::setMass(float mass) {
	m_rbody->setMass(mass);
}

float Rigidbody::getMass() {
	return m_rbody->getMass();
}

physx::PxRigidDynamic* Rigidbody::getHandle() {
	return m_rbody;
}

bool Rigidbody::serialize(YAML::Emitter& out) {
	out << YAML::Key << "HasGravity" << hasGravity();
	out << YAML::Key << "Mass" << getMass();
	return true;
}

bool Rigidbody::deserialize(YAML::Node& data) {
	setGravity(data["HasGravity"].as<bool>());
	setMass(data["Mass"].as<float>());
	return true;
}

ADERITE_PHYSICS_NAMESPACE_END
