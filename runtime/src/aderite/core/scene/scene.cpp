#include "scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"
#include "aderite/core/scene/entity.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
	namespace scene {

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

		bool scene::serialize(const std::string& path) {
			YAML::Emitter out;
			out << YAML::BeginMap; // Root

			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Name" << YAML::Value << m_name;
			out << YAML::Key << "Type" << YAML::Value << "Scene";

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
			out << YAML::EndMap; // Root

			std::ofstream fout(path);
			fout << out.c_str();

			return true;
		}

		bool scene::deserialize(const std::string& path) {
			YAML::Node data = YAML::LoadFile(path);

			// Check version
			if (!data["Version"]) {
				LOG_ERROR("Loading scene from {0} failed because there is no version information", path);
				return false;
			}

			// Check type
			if (!data["Type"]) {
				LOG_ERROR("Loading scene from {0} failed because no type information was given", path);
				return false;
			}

			if (data["Type"].as<std::string>() != "Scene") {
				LOG_ERROR("Trying to load asset of type {0} as a scene. File {1}", data["Type"].as<std::string>(), path);
				return false;
			}

			m_name = data["Name"].as<std::string>();

			// Assets
			for (auto asset : data["Assets"]) {
				// Ignore Start and Stride cause this is non binary format
				// Order asset manager to load asset metainfo
				std::string file = asset["File"].as<std::string>();
				asset::asset_base* pAsset = engine::get_asset_manager()->read_asset(file);

				if (!pAsset) {
					LOG_ERROR("Failed to load scene {0} cause asset {1} failed to be read", m_name, file);
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

	}
}
