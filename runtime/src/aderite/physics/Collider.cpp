#include "Collider.hpp"

#include <PxRigidActor.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/physics/PhysicsController.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

Collider::Collider(physx::PxGeometry* geometry) {
	physx::PxPhysics* physics = ::aderite::Engine::getPhysicsController()->getPhysics();
	physx::PxMaterial* defaultMaterial = ::aderite::Engine::getPhysicsController()->getDefaultMaterial();
	physx::PxShapeFlags baseFlags =
		physx::PxShapeFlag::eVISUALIZATION |
		physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
		physx::PxShapeFlag::eSIMULATION_SHAPE;

	p_shape = physics->createShape(
		*geometry,
		*defaultMaterial,
		true,
		baseFlags
	);

	if (p_shape == nullptr) {
		LOG_ERROR("Failed to create collider shape");
	}

	delete geometry;
}

Collider::~Collider() {
	if (p_shape != nullptr) {
		physx::PxRigidActor* prevActor = p_shape->getActor();
		if (prevActor != nullptr) {
			prevActor->detachShape(*p_shape);
		}

		p_shape->release();
	}
}

bool Collider::isTrigger() const {
	return p_shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;
}

void Collider::setTrigger(bool value) {
	// Reset
	p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

	// Set
	p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !value);
	p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, value);
}

void Collider::setMaterial(asset::Asset* material) {
	m_material = material;

	//p_shape->setMaterial();
}

void Collider::setActor(physx::PxRigidActor* actor) {
	physx::PxRigidActor* prevActor = p_shape->getActor();
	if (prevActor != nullptr) {
		p_shape->acquireReference();
		prevActor->detachShape(*p_shape);
	}
	actor->attachShape(*p_shape);
}

asset::Asset* Collider::getMaterial() const {
	return m_material;
}

bool Collider::serialize(YAML::Emitter& out) {
	// TODO: Error check
	out << YAML::Key << "IsTrigger" << YAML::Value << isTrigger();

	if (m_material != nullptr) {
		out << YAML::Key << "Material" << YAML::Value << m_material->getName();
	}

	return true;
}

bool Collider::deserialize(YAML::Node& data) {
	setTrigger(data["IsTrigger"].as<bool>());

	if (data["Material"]) {
		asset::Asset* material = ::aderite::Engine::getAssetManager()->getOrRead(data["Material"].as<std::string>());
		setMaterial(material);
	}

	return true;
}

ADERITE_PHYSICS_NAMESPACE_END
