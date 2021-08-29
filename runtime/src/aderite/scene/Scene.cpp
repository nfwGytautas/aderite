#include "Scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/MaterialAsset.hpp" 
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntityCamera.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/Rigidbody.hpp"
#include "aderite/physics/ColliderList.hpp"

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

		// TODO: Error check
		rigidbodyComponent.Body->serialize(out);

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
		TransformComponent.Rotation = transform_node["Rotation"].as<glm::vec3>();
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
		cameraComponent.Camera = new EntityCamera(e);
		cameraComponent.Camera->deserialize(cam_node);
		scene->attachCamera(cameraComponent.Camera);
	}

	// Rigid body
	auto rb_node = e_node["Rigidbody"];
	if (rb_node) {
		auto& rbodyComponent = e.addComponent<components::RigidbodyComponent>();
		aderite::Engine::getPhysicsController()->attachRigidBody(e);

		// TODO: Error check
		rbodyComponent.Body->deserialize(rb_node);
	}

	// Colliders
	auto colliders = e_node["Colliders"];
	if (colliders) {
		auto& collidersComponent = e.addComponent<components::CollidersComponent>();
		collidersComponent.Colliders = aderite::Engine::getPhysicsController()->beginNewColliderList();

		// TODO: Error check
		collidersComponent.Colliders->deserialize(colliders);

		// TODO: Rethink this
		if (e.hasComponent<components::RigidbodyComponent>()) {
			auto& rbodyComponent = e.getComponent<components::RigidbodyComponent>();
			collidersComponent.Colliders->assignToRigidbody(rbodyComponent.Body);
		}
	}

	return e;
}

Scene::~Scene() {
	for (auto& camera : m_cameras) {
		delete camera;
	}
}

void Scene::update(float delta) {
	for (auto& camera : m_cameras) {
		camera->update(delta);
	}
}

Entity Scene::createEntity(const components::MetaComponent& MetaComponent) {
	// TODO: Check for name conflicts
	Entity e = Entity(m_registry.create(), this);
	e.addComponent<components::MetaComponent>(MetaComponent);
	return e;
}

void Scene::destroyEntity(Entity entity) {
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

std::vector<interfaces::ICamera*> Scene::getCameras() {
	return m_cameras;
}

void Scene::attachCamera(interfaces::ICamera* camera) {
	m_cameras.push_back(camera);
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
		asset->load();
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

ADERITE_SCENE_NAMESPACE_END
