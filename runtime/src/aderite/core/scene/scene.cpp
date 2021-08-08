#include "scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"
#include "aderite/core/scene/entity.hpp"

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

namespace aderite {
	namespace scene {

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

		void serialize_entity(YAML::Emitter& out, entity e) {
			out << YAML::BeginMap; // Entity

			out << YAML::Key << "Entity" << YAML::Value << (entt::id_type)e;

			// Meta component
			if (e.has_component<components::meta>()) {
				out << YAML::Key << "Meta";
				out << YAML::BeginMap; // Meta

				components::meta& meta = e.get_component<components::meta>();
				out << YAML::Key << "Name" << YAML::Value << meta.Name;

				out << YAML::EndMap; // Meta
			}
			else {
				LOG_ERROR("Tried to serialize entity without meta component, this should never happen");
				return;
			}

			// Serialize rest of components

			// Transform
			if (e.has_component<components::transform>()) {
				out << YAML::Key << "Transform";
				out << YAML::BeginMap; // Transform

				components::transform& transform = e.get_component<components::transform>();
				out << YAML::Key << "Position" << YAML::Value << transform.Position;
				out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
				out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

				out << YAML::EndMap; // Transform
			}

			// Mesh renderer
			if (e.has_component<components::mesh_renderer>()) {
				out << YAML::Key << "MeshRenderer";
				out << YAML::BeginMap; // MeshRenderer

				components::mesh_renderer& mesh_renderer = e.get_component<components::mesh_renderer>();

				if (mesh_renderer.MeshHandle) {
					out << YAML::Key << "Mesh" << mesh_renderer.MeshHandle->get_name();
				}

				if (mesh_renderer.MaterialHandle) {
					out << YAML::Key << "Material" << mesh_renderer.MaterialHandle->get_name();
				}

				out << YAML::EndMap; // MeshRenderer
			}

			out << YAML::EndMap; // Entity
		}

		entity deserialize_entity(YAML::Node& e_node, scene* scene) {
			// Read meta info
			YAML::Node meta_node = e_node["Meta"];
			if (!meta_node) {
				LOG_ERROR("Tried to deserialize entity without meta component, this should never happen");
				return entity(entt::null, nullptr);
			}

			components::meta meta;
			meta.Name = meta_node["Name"].as<std::string>();

			// Create entity
			entity e = scene->create_entity(meta);

			// Deserialize rest of components

			// Transform
			auto transform_node = e_node["Transform"];
			if (transform_node) {
				auto& transform = e.add_component<components::transform>();
				transform.Position = transform_node["Position"].as<glm::vec3>();
				transform.Rotation = transform_node["Rotation"].as<glm::vec3>();
				transform.Scale = transform_node["Scale"].as<glm::vec3>();
			}

			// Mesh renderer
			auto mr_node = e_node["MeshRenderer"];
			if (mr_node) {
				auto& mesh_renderer = e.add_component<components::mesh_renderer>();
				
				if (mr_node["Mesh"]) {
					mesh_renderer.MeshHandle = engine::get_asset_manager()->get_by_name(mr_node["Mesh"].as<std::string>());
				}

				if (mr_node["Material"]) {
					mesh_renderer.MaterialHandle = engine::get_asset_manager()->get_by_name(mr_node["Material"].as<std::string>());
				}
			}

			return e;
		}

		entity scene::create_entity(const components::meta& meta) {
			// TODO: Check for name conflicts
			entity e = entity(m_registry.create(), this);
			e.add_component<components::meta>(meta);
			return e;
		}

		void scene::destroy_entity(entity entity) {
			m_registry.destroy(entity);
		}

		void scene::use_asset(asset::asset_base* asset) {
			m_assets.push_back(asset);
		}

		void scene::remove_asset(asset::asset_base* asset) {
			m_assets.erase(std::find(m_assets.begin(), m_assets.end(), asset));
		}

		bool scene::serialize(YAML::Emitter& out) {
			// Used assets
			out << YAML::Key << "Assets" << YAML::BeginSeq; // Assets

			for (auto& asset : m_assets) {
				out << YAML::BeginMap; // Asset
				out << YAML::Key << "File" << YAML::Value << asset->get_name();

				// No packing for non binary files
				out << YAML::Key << "Start" << YAML::Value << 0;
				out << YAML::Key << "Stride" << YAML::Value << 0;

				out << YAML::EndMap; // Asset
			}

			out << YAML::EndSeq; // Assets

			out << YAML::Key << "Entities" << YAML::BeginSeq; // Entities

			m_registry.each([&](auto entity_id) {
				entity e = entity(entity_id, this);

				if (!e) {
					return;
				}

				// TODO: Error check
				serialize_entity(out, e);
			});

			out << YAML::EndSeq; // Entities

			return true;
		}

		bool scene::deserialize(YAML::Node& data) {
			// Assets
			for (auto asset : data["Assets"]) {
				// Ignore Start and Stride cause this is non binary format
				// Order asset manager to load asset metainfo
				std::string file = asset["File"].as<std::string>();
				asset::asset_base* pAsset = engine::get_asset_manager()->read_asset(file);

				if (!pAsset) {
					LOG_ERROR("Failed to load scene {0} cause asset {1} failed to be read", p_name, file);
					return false;
				}

				m_assets.push_back(pAsset);
			}

			// Entities
			auto entities = data["Entities"];
			if (entities) {
				for (auto entity : entities) {
					// Error check
					deserialize_entity(entity, this);
				}
			}

			return true;
		}

		void scene::prepare_load() {
			for (asset::asset_base* asset : m_assets) {
				asset->prepare_load();
			}
		}

		bool scene::ready_to_load() {
			for (asset::asset_base* asset : m_assets) {
				if (!asset->ready_to_load()) {
					return false;
				}
			}

			return true;
		}

		void scene::load() {
			for (asset::asset_base* asset : m_assets) {
				asset->load();
			}
		}

		void scene::unload() {
			for (asset::asset_base* asset : m_assets) {
				asset->unload();
			}
		}

		bool scene::is_preparing() {
			for (asset::asset_base* asset : m_assets) {
				if (!asset->is_preparing()) {
					return false;
				}
			}

			return true;
		}

		asset::asset_type scene::type() const {
			return asset::asset_type::SCENE;
		}

		bool scene::in_group(asset::asset_group group) const {
			switch (group) {
			case asset::asset_group::SHADER:
				return true;
			default:
				return false;
			}
		}

	}
}
