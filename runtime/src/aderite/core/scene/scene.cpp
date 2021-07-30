#include "scene.hpp"

#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"

// Previous versions:
//	- 2021_07_14r1

constexpr const char* current_version = "2021_07_14r1";

namespace aderite {
	namespace scene {

		void scene::use_asset(relay_ptr<asset::asset_base> asset) {
			m_assets.push_back(asset->get_handle());
		}

		void scene::remove_asset(relay_ptr<asset::asset_base> asset) {
			m_assets.erase(std::find(m_assets.begin(), m_assets.end(), asset->get_handle()));
		}

		bool scene::serialize(const std::string& path) {
			YAML::Emitter out;
			out << YAML::BeginMap; // Root

			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Handle" << YAML::Value << m_handle;
			out << YAML::Key << "Name" << YAML::Value << m_name;
			out << YAML::Key << "Type" << YAML::Value << "Scene";

			// Used assets
			out << YAML::Key << "Assets" << YAML::BeginSeq; // Assets

			for (auto& asset : m_assets) {
				out << YAML::Value << asset;
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

			m_handle = data["Handle"].as<std::string>();
			m_name = data["Name"].as<std::string>();

			for (auto asset : data["Assets"]) {
				m_assets.push_back(asset.as<std::string>());
			}

			return true;
		}

	}
}
