#include "scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
	namespace scene {

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
