#include "asset_manager.hpp"

#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/random.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/core/assets/object/shader_asset.hpp"
#include "aderite/core/assets/object/material_asset.hpp"
#include "aderite/core/assets/object/mesh_asset.hpp"
#include "aderite/core/assets/sources/mesh_source.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
	namespace asset {

		bool asset_manager::init() {
			// Setup depending on the resource save type

			// Binary save type is compressed into specific raw, scene, asset chunks
			// this type is optimized for a specific platform and build and is not
			// cross-platform.
#if BINARY_SAVE_TYPE == 1
			LOG_ERROR("Binary save type is not supported");
			return false;
#endif

			// Human readable type is just yaml files split into description and raw
			// data files
#if HUMAN_READABLE_SAVE_TYPE == 1
			LOG_DEBUG("Human readable save type");
#endif

			return true;
		}

		void asset_manager::shutdown() {
			unload_all();
		}

		void asset_manager::set_root_dir(const std::string& path) {
			m_rootDir = std::filesystem::path(path);
			std::filesystem::path res_dir = get_res_dir();
			std::filesystem::path raw_dir = get_raw_dir();

			LOG_TRACE("Setting root directory to {0}", m_rootDir.string());

			// Create directories if they don't exist
			if (!std::filesystem::is_directory(res_dir) || !std::filesystem::exists(res_dir)) {
				std::filesystem::create_directory(res_dir);
			}

			if (!std::filesystem::is_directory(raw_dir) || !std::filesystem::exists(raw_dir)) {
				std::filesystem::create_directory(raw_dir);
			}
		}
		
		asset_base* asset_manager::get_by_name(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](asset_base* asset) {
				return asset->get_name() == name;
			});

			if (it == m_assets.end()) {
				LOG_WARN("Tried to get non existing asset with name {0}", name);
				return nullptr;
			}

			return *it;
		}

		bool asset_manager::has(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](asset_base* asset) {
				return asset->get_name() == name;
			});

			return it != m_assets.end();
		}

		std::string asset_manager::load_txt_file(const std::string& path) {
			std::ifstream fin;
			std::stringstream fstream;

			fin.open(get_raw_dir() / path);

			if (!fin)
			{
				LOG_ERROR("Couldn't open the file {0}", path);
				return "";
			}

			fstream << fin.rdbuf();
			fin.close();

			return fstream.str();
		}

		std::vector<unsigned char> asset_manager::load_bin_file(const std::string& path) {
			// TODO: Error check
			std::ifstream file(get_raw_dir() / path, std::ios::binary);
			file.unsetf(std::ios::skipws);

			std::streampos fileSize;
			file.seekg(0, std::ios::end);
			fileSize = file.tellg();
			file.seekg(0, std::ios::beg);

			std::vector<unsigned char> vec;
			vec.reserve(fileSize);
			vec.insert(vec.begin(),
				std::istream_iterator<unsigned char>(file),
				std::istream_iterator<unsigned char>());

			return vec;
		}

		void asset_manager::load_mesh_source(const std::string& path, std::function<void(mesh_source*)> loaded) {
			LOG_WARN("load_mesh_source not async yet");
			loaded(new mesh_source(get_raw_dir() / path));
		}

		asset::asset_base* asset_manager::read_asset(const std::string& path) {
			LOG_TRACE("Asset manager reading {0}", path);

			// Check for duplication
			if (has(path)) {
				LOG_WARN("Requested double read");
				return get_by_name(path);
			}

			// Open YAML reader
			YAML::Node data = YAML::LoadFile((get_res_dir() / path).string());

			// Check version
			if (!data["Version"]) {
				LOG_ERROR("Loading asset from {0} failed because there is no version information", path);
				return nullptr;
			}

			// Check type
			if (!data["Type"]) {
				LOG_ERROR("Loading asset from {0} failed because no type information was given", path);
				return nullptr;
			}

			// Check that the name is the same
			if (!data["Name"]) {
				LOG_ERROR("Loading asset from {0} failed because no name information was given", path);
				return nullptr;
			}

			// Check that the name is the same
			std::string name = data["Name"].as<std::string>();
			if (name != path) {
				LOG_WARN("Asset {0} was previously moved incorrectly, cause recorded name is {1}", path, name);
				return nullptr;
			}

			// Now deserialize
			asset_type type = (asset_type)(data["Type"].as<size_t>());
			asset::asset_base* asset = nullptr;

			if (type == asset_type::SCENE) {
				asset = new scene::scene(name);
			}
			else if (type == asset_type::SHADER) {
				asset = new asset::shader_asset(name);
			}
			else if (type == asset_type::MATERIAL) {
				asset = new asset::material_asset(name);
			}
			else if (type == asset_type::MESH) {
				asset = new asset::mesh_asset(name);
			}
			else {
				LOG_ERROR("Unknown asset type {0}", type);
				return nullptr;
			}

			if (!asset->deserialize(data)) {
				LOG_ERROR("Failed to deserialize asset from {0}", name);
				delete asset;
				return nullptr;
			}

			m_assets.push_back(asset);
			return asset;
		}
		
		asset::asset_base* asset_manager::get_or_read(const std::string& name) {
			if (!has(name)) {
				return read_asset(name);
			}
			else {
				return get_by_name(name);
			}
		}

		void asset_manager::save_asset(asset::asset_base* asset) {
			YAML::Emitter out;

			out << YAML::BeginMap;

			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Name" << YAML::Value << asset->get_name();
			out << YAML::Key << "Type" << YAML::Value << (size_t)asset->type();

			if (!asset->serialize(out)) {
				LOG_ERROR("Failed to save asset {0}", asset->get_name());
				return;
			}

			out << YAML::EndMap;

			std::ofstream fout(get_res_dir() / asset->get_name());
			fout << out.c_str();
		}

		void asset_manager::unload_all() {
			// Unload all then delete
			for (auto& m_asset : m_assets) {
				m_asset->unload();
			}

			for (auto& m_asset : m_assets) {
				delete m_asset;
			}

			m_assets.clear();
		}

		void asset_manager::unload(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](asset_base* asset) {
				return asset->get_name() == name;
			});

			if (it == m_assets.end()) {
				return;
			}

			delete (*it);
			m_assets.erase(it);
		}

		bool asset_manager::can_create(const std::string& name) {
			// Check for conflicts (editor only thing so this will never be called from scripts / runtime so no need to check for binary format)
			if (std::filesystem::exists(get_res_dir() / name)) {
				return false;
			}

			return true;
		}
	}
}
