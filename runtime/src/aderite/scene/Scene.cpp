#include "Scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/rendering/Pipeline.hpp"


namespace aderite {
namespace scene {

void serialize_entity(YAML::Emitter& out, Entity e) {
	out << YAML::BeginMap; // Entity

	out << YAML::Key << "Entity" << YAML::Value << (entt::id_type)e;

	// Meta component
	if (e.hasComponent<components::MetaComponent>()) {
		out << YAML::Key << "Meta";
		out << YAML::BeginMap; // Meta

		components::MetaComponent& MetaComponent = e.getComponent<components::MetaComponent>();
		out << YAML::Key << "Name" << YAML::Value << MetaComponent.Name;

		out << YAML::EndMap; // Meta
	}
	else {
		LOG_ERROR("Tried to serialize Entity without MetaComponent component, this should never happen");
		return;
	}

	// Serialize rest of components

	// Transform
	if (e.hasComponent<components::TransformComponent>()) {
		out << YAML::Key << "Transform";
		out << YAML::BeginMap; // Transform

		components::TransformComponent& TransformComponent = e.getComponent<components::TransformComponent>();
		out << YAML::Key << "Position" << YAML::Value << TransformComponent.Position;
		out << YAML::Key << "Rotation" << YAML::Value << TransformComponent.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << TransformComponent.Scale;

		out << YAML::EndMap; // Transform
	}

	// Mesh Renderer
	if (e.hasComponent<components::MeshRendererComponent>()) {
		out << YAML::Key << "MeshRenderer";
		out << YAML::BeginMap; // MeshRenderer

		components::MeshRendererComponent& MeshRendererComponent = e.getComponent<components::MeshRendererComponent>();

		if (MeshRendererComponent.MeshHandle) {
			out << YAML::Key << "Mesh" << MeshRendererComponent.MeshHandle->getHandle();
		}

		if (MeshRendererComponent.MaterialHandle) {
			out << YAML::Key << "Material" << MeshRendererComponent.MaterialHandle->getHandle();
		}

		out << YAML::EndMap; // MeshRenderer
	}

	// Camera
	if (e.hasComponent<components::CameraComponent>()) {
		out << YAML::Key << "Camera";
		out << YAML::BeginMap; // Camera

		components::CameraComponent& cameraComponent = e.getComponent<components::CameraComponent>();
		
		// TODO: Serialize camera

		out << YAML::EndMap; // Camera
	}

	// Rigid body
	if (e.hasComponent<components::RigidbodyComponent>()) {
		out << YAML::Key << "Rigidbody";
		out << YAML::BeginMap; // Rigidbody

		components::RigidbodyComponent& rigidbodyComponent = e.getComponent<components::RigidbodyComponent>();

		out << YAML::Key << "IsKinematic" << rigidbodyComponent.IsKinematic;
		out << YAML::Key << "HasGravity" << rigidbodyComponent.HasGravity;
		out << YAML::Key << "Mass" << rigidbodyComponent.Mass;

		out << YAML::EndMap; // Rigidbody
	}

	// Colliders
	if (e.hasComponent<components::CollidersComponent>()) {
		out << YAML::Key << "Colliders";
		out << YAML::BeginSeq; // Colliders

		components::CollidersComponent& collidersComponent = e.getComponent<components::CollidersComponent>();

		// TODO: Error check
		::aderite::Engine::getSerializer()->writeUntrackedType(out, collidersComponent.Colliders);

		out << YAML::EndSeq; // Colliders
	}

	// Audio sources
	if (e.hasComponent<components::AudioSourceComponent>()) {
		out << YAML::Key << "AudioSource";
		out << YAML::BeginSeq; // AudioSource

		components::AudioSourceComponent& audioSourcesComponent = e.getComponent<components::AudioSourceComponent>();

		// TODO: Serialize

		out << YAML::EndSeq; // AudioSource
	}

	// Audio listener
	if (e.hasComponent<components::AudioListenerComponent>()) {
		out << YAML::Key << "AudioListener";
		out << YAML::BeginMap; // AudioListener

		components::AudioListenerComponent& audioListenerComponent = e.getComponent<components::AudioListenerComponent>();

		out << YAML::Key << "IsEnabled" << audioListenerComponent.IsEnabled;

		out << YAML::EndMap; // AudioListener
	}

	out << YAML::EndMap; // Entity
}

Entity deserialize_entity(YAML::Node& e_node, Scene* scene) {
	// Read MetaComponent info
	YAML::Node meta_node = e_node["Meta"];
	if (!meta_node) {
		LOG_ERROR("Tried to deserialize Entity without MetaComponent component, this should never happen");
		return Entity(entt::null, nullptr);
	}

	components::MetaComponent MetaComponent;
	MetaComponent.Name = meta_node["Name"].as<std::string>();

	// Create Entity
	Entity e = scene->createEntity(MetaComponent);

	// Deserialize rest of components

	// Transform
	auto transform_node = e_node["Transform"];
	if (transform_node) {
		auto& TransformComponent = e.addComponent<components::TransformComponent>();
		TransformComponent.Position = transform_node["Position"].as<glm::vec3>();
		TransformComponent.Rotation = transform_node["Rotation"].as<glm::quat>();
		TransformComponent.Scale = transform_node["Scale"].as<glm::vec3>();
	}

	// Mesh Renderer
	auto mr_node = e_node["MeshRenderer"];
	if (mr_node) {
		auto& MeshRendererComponent = e.addComponent<components::MeshRendererComponent>();
		
		if (mr_node["Mesh"]) {
			const io::SerializableHandle handle = mr_node["Mesh"].as<io::SerializableHandle>();
			MeshRendererComponent.MeshHandle = static_cast<asset::MeshAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
			ADERITE_DYNAMIC_ASSERT(MeshRendererComponent.MeshHandle != nullptr, "Tried to use a deleted asset");
		}

		if (mr_node["Material"]) {
			const io::SerializableHandle handle = mr_node["Material"].as<io::SerializableHandle>();
			MeshRendererComponent.MaterialHandle = static_cast<asset::MaterialAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
			ADERITE_DYNAMIC_ASSERT(MeshRendererComponent.MaterialHandle != nullptr, "Tried to use a deleted asset");
		}
	}

	// Camera
	auto cam_node = e_node["Camera"];
	if (cam_node) {
		auto& cameraComponent = e.addComponent<components::CameraComponent>();
		
		// TODO: Deserialize camera
	}

	// Rigid body
	auto rb_node = e_node["Rigidbody"];
	if (rb_node) {
		auto& rbodyComponent = e.addComponent<components::RigidbodyComponent>();

		// TODO: Error check
		rbodyComponent.IsKinematic = rb_node["IsKinematic"].as<bool>();
		rbodyComponent.HasGravity = rb_node["HasGravity"].as<bool>();
		rbodyComponent.Mass = rb_node["Mass"].as<float>();
	}

	// Colliders
	auto colliders = e_node["Colliders"];
	if (colliders) {
		if (!e.hasComponent<components::CollidersComponent>()) {
			e.addComponent<components::CollidersComponent>();
		}

		auto& collidersComponent = e.getComponent<components::CollidersComponent>();

		// TODO: Error check
		collidersComponent.Colliders = static_cast<physics::ColliderList*>(::aderite::Engine::getSerializer()->parseUntrackedType(e_node));
	}

	// Audio sources
	auto audioSources = e_node["AudioSource"];
	if (audioSources) {
		auto& audioSourcesComponent = e.addComponent<components::AudioSourceComponent>();

		// TODO: Deserialize
		audioSourcesComponent.Instance = ::aderite::Engine::getAudioController()->createAudioInstance("TODO");
	}

	// Audio listener
	auto al_node = e_node["AudioListener"];
	if (al_node) {
		auto& rbodyComponent = e.addComponent<components::AudioListenerComponent>();

		// TODO: Error check
		rbodyComponent.IsEnabled = al_node["IsEnabled"].as<bool>();
	}

	return e;
}

Scene::~Scene() {
	auto entities = m_registry.view<scene::components::CollidersComponent>();
	for (auto entity : entities) {
		auto [colliders] = entities.get(entity);
		delete colliders.Colliders;
	}

	m_physicsScene->release();
}

void Scene::update(float delta) {
	auto cameraGroup = m_registry
		.group<scene::components::CameraComponent>(
			entt::get<scene::components::TransformComponent>);
	for (auto entity : cameraGroup) {
		auto [camera, transform] = cameraGroup.get(entity);

		// TODO: Update entity cameras
	}
}

void Scene::fixedUpdate(float step) {
	// Sync ECS to physics
	syncEcsToPhysics();
	m_physicsScene->simulate(step);
	m_physicsScene->fetchResults(true);
	syncPhysicsToEcs();
}

Entity Scene::createEntity(const components::MetaComponent& MetaComponent) {
	// TODO: Check for name conflicts
	Entity e = Entity(m_registry.create(), this);
	e.addComponent<components::MetaComponent>(MetaComponent);
	return e;
}

void Scene::destroyEntity(Entity entity) {
	if (entity.hasComponent<components::CollidersComponent>()) {
		delete entity.getComponent<components::CollidersComponent>().Colliders;
	}

	m_registry.destroy(entity);
}

physx::PxScene* Scene::getPhysicsScene() const {
	return m_physicsScene;
}

rendering::Pipeline* Scene::getPipeline() const {
	return m_pipeline;
}

void Scene::setPipeline(rendering::Pipeline* pipeline) {
	m_pipeline = pipeline;
}

reflection::Type Scene::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::SCENE);
}

bool Scene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	// Entities
	emitter << YAML::Key << "Entities" << YAML::BeginSeq;

	m_registry.each([&](auto entity_id) {
		Entity e = Entity(entity_id, this);

		if (!e) {
			return;
		}

		// TODO: Error check
		serialize_entity(emitter, e);
	});

	emitter << YAML::EndSeq; // Entities

	emitter << YAML::Key << "Pipeline" << YAML::Value;
	if (m_pipeline != nullptr) {
		emitter << m_pipeline->getHandle();
	}
	else {
		emitter << YAML::Null;
	}

	return true;
}

bool Scene::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	// Entities
	auto entities = data["Entities"];
	if (entities) {
		for (auto Entity : entities) {
			// Error check
			deserialize_entity(Entity, this);
		}
	}

	if (!data["Pipeline"].IsNull()) {
		setPipeline(static_cast<rendering::Pipeline*>(::aderite::Engine::getSerializer()->getOrRead(data["Pipeline"].as<io::SerializableHandle>())));
	}

	return true;
}

Scene::Scene() {
	auto physics = ::aderite::Engine::getPhysicsController()->getPhysics();

	// TODO: Configure physics properties
	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = ::aderite::Engine::getPhysicsController()->getDispatcher();
	sceneDesc.filterShader = physics::PhysicsController::filterShader;
	sceneDesc.simulationEventCallback = this;
	//sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	m_physicsScene = physics->createScene(sceneDesc);
	ADERITE_DYNAMIC_ASSERT(m_physicsScene != nullptr, "Failed to create a PhysX scene");
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component) {}

template<>
void Scene::onComponentAdded<components::RigidbodyComponent>(Entity entity, components::RigidbodyComponent& component) {
	// Can't have rigidbody without colliders
	if (!entity.hasComponent<components::CollidersComponent>()) {
		entity.addComponent<components::CollidersComponent>();
	}

	// Convert to static
	auto& colliders = entity.getComponent<components::CollidersComponent>();

	// Convert to dynamic
	physx::PxRigidDynamic* actor = ::aderite::Engine::getPhysicsController()->createDynamicBody();

	// Switch
	actor->userData = &entity.getComponent<components::MetaComponent>();
	m_physicsScene->removeActor(*colliders.Colliders->getActor());
	colliders.Colliders->setActor(actor);
	m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<components::CollidersComponent>(Entity entity, components::CollidersComponent& component) {
	component.Colliders = new physics::ColliderList();
	physx::PxRigidStatic* actor = ::aderite::Engine::getPhysicsController()->createStaticBody();
	actor->userData = &entity.getComponent<components::MetaComponent>();
	component.Colliders->setActor(actor);
	m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<components::AudioSourceComponent>(Entity entity, components::AudioSourceComponent& component) {
	// Add transform if don't have already
	if (!entity.hasComponent<components::TransformComponent>()) {
		entity.addComponent<components::TransformComponent>();
	}
}	

template<>
void Scene::onComponentAdded<components::AudioListenerComponent>(Entity entity, components::AudioListenerComponent& component) {
	// Add transform if don't have already
	if (!entity.hasComponent<components::TransformComponent>()) {
		entity.addComponent<components::TransformComponent>();
	}
}

template<>
void Scene::onComponentAdded<components::CameraComponent>(Entity entity, components::CameraComponent& component) {
	// Add transform if don't have already
	if (!entity.hasComponent<components::TransformComponent>()) {
		entity.addComponent<components::TransformComponent>();
	}
}

template<typename T>
void Scene::onComponentRemoved(Entity entity, T& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::TransformComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::MeshRendererComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::AudioSourceComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::AudioListenerComponent& component) {}

template<>
void Scene::onComponentRemoved<components::RigidbodyComponent>(Entity entity, components::RigidbodyComponent& component) {
	// Convert to static
	auto& colliders = entity.getComponent<components::CollidersComponent>();

	// Make static
	physx::PxRigidStatic* actor = ::aderite::Engine::getPhysicsController()->createStaticBody();

	// Switch
	m_physicsScene->removeActor(*colliders.Colliders->getActor());
	colliders.Colliders->setActor(actor);
	m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentRemoved<components::CollidersComponent>(Entity entity, components::CollidersComponent& component) {
	if (entity.hasComponent<components::RigidbodyComponent>()) {
		entity.removeComponent<components::RigidbodyComponent>();
	}
	m_physicsScene->removeActor(*component.Colliders->getActor());
	delete component.Colliders;
}

void Scene::onContact(
	const physx::PxContactPairHeader& pairHeader,
	const physx::PxContactPair* pairs,
	physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++) {
		const physx::PxContactPair& cp = pairs[i];

		physx::PxRigidActor* actor1 = pairHeader.actors[0];
		physx::PxRigidActor* actor2 = pairHeader.actors[1];

		components::MetaComponent* e1 = static_cast<components::MetaComponent*>(actor1->userData);
		components::MetaComponent* e2 = static_cast<components::MetaComponent*>(actor2->userData);

		if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			// Collision enter
			LOG_INFO("{0} colliding with {1}", e1->Name, e2->Name);
		}
		else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			// Collision leave
			LOG_INFO("{0} no longer colliding with {1}", e1->Name, e2->Name);
		}
	}
}

void Scene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
	for (physx::PxU32 i = 0; i < nbPairs; i++) {
		const physx::PxTriggerPair& cp = pairs[i];

		physx::PxRigidActor* actor = cp.otherActor;
		physx::PxRigidActor* trigger = cp.triggerActor;

		components::MetaComponent* metaActor = static_cast<components::MetaComponent*>(actor->userData);
		components::MetaComponent* metaTrigger = static_cast<components::MetaComponent*>(trigger->userData);

		if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			LOG_WARN("{0} entered {1} trigger", metaActor->Name, metaTrigger->Name);
		}
		else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			LOG_WARN("{0} left {1} trigger", metaActor->Name, metaTrigger->Name);
		}
	}
}

void Scene::syncActorToEcs(
	physx::PxRigidActor* actor,
	const scene::components::CollidersComponent& colliders,
	const scene::components::TransformComponent& transform)
{
	// Transform
	actor->setGlobalPose(
		physx::PxTransform(
			physx::PxVec3{ transform.Position.x, transform.Position.y, transform.Position.z },
			physx::PxQuat{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w }
	));

	// Scale
	colliders.Colliders->setScale(transform.Scale);

	// Collision group
	physx::PxSetGroup(*actor, 0);
}

void Scene::syncEcsToPhysics() {
	// Sync changes with ECS
	auto dynamicGroup = m_registry.group<scene::components::RigidbodyComponent>(
			entt::get<scene::components::CollidersComponent, scene::components::TransformComponent>);
	for (auto entity : dynamicGroup) {
		auto [rigidbody, colliders, transform] = dynamicGroup.get<
			scene::components::RigidbodyComponent,
			scene::components::CollidersComponent,
			scene::components::TransformComponent>(entity);

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(colliders.Colliders->getActor());
		if (transform.WasAltered || rigidbody.WasAltered) {
			// Sync ECS -> PhysX
			syncActorToEcs(actor, colliders, transform);

			// Settings
			actor->setMass(rigidbody.Mass);
			actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rigidbody.HasGravity);
			actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidbody.IsKinematic);

			// Reset flags
			transform.WasAltered = false;
			rigidbody.WasAltered = false;
		}
	}

	auto staticGroup = m_registry.group<scene::components::CollidersComponent>(
			entt::get<scene::components::TransformComponent>,
			entt::exclude<scene::components::RigidbodyComponent>);
	for (auto entity : staticGroup) {
		auto [colliders, transform] = staticGroup.get<
			scene::components::CollidersComponent,
			scene::components::TransformComponent>(entity);

		physx::PxRigidStatic* actor = static_cast<physx::PxRigidStatic*>(colliders.Colliders->getActor());
		if (transform.WasAltered) {
			LOG_WARN("Altered position of static physics entity");

			// Sync ECS -> PhysX
			syncActorToEcs(actor, colliders, transform);

			// Reset flags
			transform.WasAltered = false;
		}
	}
}

void Scene::syncPhysicsToEcs() {
	// Sync changes with physics
	auto physicsGroup = m_registry.group<scene::components::CollidersComponent>(
		entt::get<scene::components::TransformComponent>);
	for (auto entity : physicsGroup) {
		auto [colliders, transform] = physicsGroup.get<
			scene::components::CollidersComponent,
			scene::components::TransformComponent>(entity);

		physx::PxRigidActor* actor = static_cast<physx::PxRigidActor*>(colliders.Colliders->getActor());
		
		// Transform
		physx::PxTransform pxt = actor->getGlobalPose();
		transform.Position = { pxt.p.x, pxt.p.y, pxt.p.z };
		transform.Rotation = { pxt.q.w,  pxt.q.x, pxt.q.y, pxt.q.z };
	}
}

}
}
