#include "Scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/MaterialAsset.hpp" 
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Components.hpp"


// YAML extensions
namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

ADERITE_SCENE_NAMESPACE_BEGIN

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

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

			if (!pAsset) {
				LOG_ERROR("Failed to load scene {0} cause asset {1} failed to be read", scene->getName(), name);
				return Entity::null();
			}

			scene->useAsset(pAsset);

			MeshRendererComponent.MeshHandle = static_cast<asset::MeshAsset*>(pAsset);
		}

		if (mr_node["Material"]) {
			const std::string name = mr_node["Material"].as<std::string>();
			asset::Asset* pAsset = ::aderite::Engine::getAssetManager()->getOrRead(name);

			if (!pAsset) {
				LOG_ERROR("Failed to load scene {0} cause asset {1} failed to be read", scene->getName(), name);
				return Entity::null();
			}

			scene->useAsset(pAsset);

			MeshRendererComponent.MaterialHandle = static_cast<asset::MaterialAsset*>(pAsset);
		}
	}

	return e;
}

Entity Scene::createEntity(const components::MetaComponent& MetaComponent) {
	// TODO: Check for name conflicts
	Entity e = Entity(m_registry.create(), this);
	e.addComponent<components::MetaComponent>(MetaComponent);
	return e;
}

void Scene::destroyEntity(Entity Entity) {
	m_registry.destroy(Entity);
}

void Scene::useAsset(asset::Asset* asset) {
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
