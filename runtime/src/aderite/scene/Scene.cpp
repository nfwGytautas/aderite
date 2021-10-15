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
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntityCamera.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/audio/AudioInstanceList.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/ColliderList.hpp"

#include "aderite/asset/TextureAsset.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

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
			out << YAML::Key << "Mesh" << MeshRendererComponent.MeshHandle->getName();
		}

		if (MeshRendererComponent.MaterialHandle) {
			out << YAML::Key << "Material" << MeshRendererComponent.MaterialHandle->getName();
		}

		out << YAML::EndMap; // MeshRenderer
	}

	// Camera
	if (e.hasComponent<components::CameraComponent>()) {
		out << YAML::Key << "Camera";
		out << YAML::BeginMap; // Camera

		components::CameraComponent& cameraComponent = e.getComponent<components::CameraComponent>();
		
		// TODO: Error check
		cameraComponent.Camera->serialize(out);

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
		collidersComponent.Colliders->serialize(out);

		out << YAML::EndSeq; // Colliders
	}

	// Audio sources
	if (e.hasComponent<components::AudioSourcesComponent>()) {
		out << YAML::Key << "AudioSources";
		out << YAML::BeginSeq; // AudioSources

		components::AudioSourcesComponent& audioSourcesComponent = e.getComponent<components::AudioSourcesComponent>();

		// TODO: Error check
		audioSourcesComponent.Instances->serialize(out);

		out << YAML::EndSeq; // AudioSources
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
			const std::string name = mr_node["Mesh"].as<std::string>();
			asset::Asset* pAsset = ::aderite::Engine::getAssetManager()->getOrRead(name);

			if (pAsset) {
				scene->useAsset(pAsset);
				MeshRendererComponent.MeshHandle = static_cast<asset::MeshAsset*>(pAsset);
			}
			else {
				LOG_WARN("Invalid asset handle received for mesh, probably the file {0} was deleted or moved externally", name);
			}
		}

		if (mr_node["Material"]) {
			const std::string name = mr_node["Material"].as<std::string>();
			asset::Asset* pAsset = ::aderite::Engine::getAssetManager()->getOrRead(name);

			if (pAsset) {
				scene->useAsset(pAsset);
				MeshRendererComponent.MaterialHandle = static_cast<asset::MaterialAsset*>(pAsset);
			}
			else {
				LOG_WARN("Invalid asset handle received for material, probably the file {0} was deleted or moved externally", name);
			}
		}
	}

	// Camera
	auto cam_node = e_node["Camera"];
	if (cam_node) {
		auto& cameraComponent = e.addComponent<components::CameraComponent>();
		cameraComponent.Camera->deserialize(cam_node);
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
		collidersComponent.Colliders->deserialize(colliders);
	}

	// Audio sources
	auto audioSources = e_node["AudioSources"];
	if (audioSources) {
		auto& audioSourcesComponent = e.addComponent<components::AudioSourcesComponent>();
		audioSourcesComponent.Instances->deserialize(audioSources);
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

	auto entities2 = m_registry.view<scene::components::AudioSourcesComponent>();
	for (auto entity : entities2) {
		auto [sources] = entities2.get(entity);
		delete sources.Instances;
	}

	auto entities3 = m_registry.view<scene::components::CameraComponent>();
	for (auto entity : entities3) {
		auto [camera] = entities3.get(entity);
		delete camera.Camera;
	}

	m_physicsScene->release();
}

void Scene::update(float delta) {
	auto cameraGroup = m_registry
		.group<scene::components::CameraComponent>(
			entt::get<scene::components::TransformComponent>);
	for (auto entity : cameraGroup) {
		auto [camera, transform] = cameraGroup.get(entity);
		camera.Camera->update(delta);
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
	if (entity.hasComponent<components::AudioSourcesComponent>()) {
		delete entity.getComponent<components::AudioSourcesComponent>().Instances;
	}

	if (entity.hasComponent<components::CollidersComponent>()) {
		delete entity.getComponent<components::CollidersComponent>().Colliders;
	}

	m_registry.destroy(entity);
}

void Scene::useAsset(asset::Asset* asset) {
	// Check if this asset is not duplicate
	for (auto usedAssets : m_assets) {
		if (usedAssets == asset) {
			return;
		}
	}

	m_assets.push_back(asset);
}

void Scene::removeAsset(asset::Asset* asset) {
	m_assets.erase(std::find(m_assets.begin(), m_assets.end(), asset));
}

bool Scene::serialize(YAML::Emitter& out) {
	// Entities
	out << YAML::Key << "Entities" << YAML::BeginSeq;

	m_registry.each([&](auto entity_id) {
		Entity e = Entity(entity_id, this);

		if (!e) {
			return;
		}

		// TODO: Error check
		serialize_entity(out, e);
	});

	out << YAML::EndSeq; // Entities

	return true;
}

bool Scene::deserialize(YAML::Node& data) {
	// Entities
	m_assets.clear();
	auto entities = data["Entities"];
	if (entities) {
		for (auto Entity : entities) {
			// Error check
			deserialize_entity(Entity, this);
		}
	}

	return true;
}

Scene::Scene(const std::string& name)
	: Asset(name + ".scene")
{
	auto physics = ::aderite::Engine::getPhysicsController()->getPhysics();

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = ::aderite::Engine::getPhysicsController()->getDispatcher();
	sceneDesc.filterShader = physics::PhysicsController::filterShader;
	sceneDesc.simulationEventCallback = this;
	//sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	m_physicsScene = physics->createScene(sceneDesc);

	if (m_physicsScene == nullptr) {
		LOG_ERROR("Failed to create a PhysX scene");
		return;
	}
}

physx::PxScene* Scene::getPhysicsScene() const {
	return m_physicsScene;
}

void Scene::prepareLoad() {
	for (asset::Asset* asset : m_assets) {
		asset->prepareLoad();
	}
}

bool Scene::isReadyToLoad() {
	for (asset::Asset* asset : m_assets) {
		if (!asset->isReadyToLoad()) {
			return false;
		}
	}

	return true;
}

void Scene::load() {
	for (asset::Asset* asset : m_assets) {
		if (!asset->isLoaded()) {
			asset->load();
		}
	}
}

void Scene::unload() {
	for (asset::Asset* asset : m_assets) {
		asset->unload();
	}
}

bool Scene::isPreparing() {
	for (asset::Asset* asset : m_assets) {
		if (!asset->isPreparing()) {
			return false;
		}
	}

	return true;
}

bool Scene::isLoaded() {
	for (asset::Asset* asset : m_assets) {
		if (!asset->isLoaded()) {
			return false;
		}
	}
	
	return true;
}

size_t Scene::hash() const {
	return std::hash<std::string>{}(p_name);
}

asset::AssetType Scene::type() const {
	return asset::AssetType::SCENE;
}

bool Scene::isInGroup(asset::AssetGroup group) const {
	switch (group) {
	default:
		return false;
	}
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
	m_physicsScene->removeActor(*colliders.Colliders->getActor());
	colliders.Colliders->setActor(actor);
	m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<components::CollidersComponent>(Entity entity, components::CollidersComponent& component) {
	component.Colliders = new physics::ColliderList(entity);
	physx::PxRigidStatic* actor = ::aderite::Engine::getPhysicsController()->createStaticBody();
	component.Colliders->setActor(actor);
	m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<components::AudioSourcesComponent>(Entity entity, components::AudioSourcesComponent& component) {
	component.Instances = new audio::AudioInstanceList();
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

	component.Camera = new EntityCamera(entity);
}

template<typename T>
void Scene::onComponentRemoved(Entity entity, T& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::TransformComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, components::MeshRendererComponent& component) {}

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

template<>
void Scene::onComponentRemoved<components::AudioSourcesComponent>(Entity entity, components::AudioSourcesComponent& component) {
	delete component.Instances;
}

template<>
void Scene::onComponentRemoved<components::CameraComponent>(Entity entity, components::CameraComponent& component) {
	delete component.Camera;
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

		Entity* e1 = static_cast<Entity*>(actor1->userData);
		Entity* e2 = static_cast<Entity*>(actor2->userData);

		auto& metaE1 = e1->getComponent<components::MetaComponent>();
		auto& metaE2 = e2->getComponent<components::MetaComponent>();

		if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			// Collision enter
			LOG_INFO("{0} colliding with {1}", metaE1.Name, metaE2.Name);
		}
		else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			// Collision leave
			LOG_INFO("{0} no longer colliding with {1}", metaE1.Name, metaE2.Name);
		}
	}
}

void Scene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
	for (physx::PxU32 i = 0; i < nbPairs; i++) {
		const physx::PxTriggerPair& cp = pairs[i];

		physx::PxRigidActor* actor = cp.otherActor;
		physx::PxRigidActor* trigger = cp.triggerActor;

		Entity* actorEntity = static_cast<Entity*>(actor->userData);
		Entity* triggerEntity = static_cast<Entity*>(trigger->userData);

		auto& metaActor = actorEntity->getComponent<components::MetaComponent>();
		auto& metaTrigger = triggerEntity->getComponent<components::MetaComponent>();

		if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			LOG_WARN("{0} entered {1} trigger", metaActor.Name, metaTrigger.Name);
		}
		else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			LOG_WARN("{0} left {1} trigger", metaActor.Name, metaTrigger.Name);
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

ADERITE_SCENE_NAMESPACE_END
