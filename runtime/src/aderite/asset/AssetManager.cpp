#include "AssetManager.hpp"

#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/random.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/MeshSource.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
	namespace asset {

		bool AssetManager::init() {
			// Setup depending on the resource save type

			// Binary save type is compressed into specific raw, Scene, asset chunks
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

		void AssetManager::shutdown() {
			unload_all();
		}

		void AssetManager::set_root_dir(const std::string& path) {
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
		
		Asset* AssetManager::get_by_name(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](Asset* asset) {
				return asset->getName() == name;
			});

			if (it == m_assets.end()) {
				LOG_WARN("Tried to get non existing asset with name {0}", name);
				return nullptr;
			}

			return *it;
		}

		bool AssetManager::has(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](Asset* asset) {
				return asset->getName() == name;
			});

			return it != m_assets.end();
		}

		std::string AssetManager::load_txt_file(const std::string& path) {
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

		std::vector<unsigned char> AssetManager::load_bin_file(const std::string& path) {
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

		void AssetManager::load_mesh_source(const std::string& path, std::function<void(mesh_source*)> loaded) {
			LOG_WARN("load_mesh_source not async yet");
			loaded(new mesh_source(get_raw_dir() / path));
		}

		asset::Asset* AssetManager::read_asset(const std::string& path) {
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
			AssetType type = (AssetType)(data["Type"].as<size_t>());
			asset::Asset* asset = nullptr;

			if (type == AssetType::SCENE) {
				asset = new scene::Scene(name);
			}
			else if (type == AssetType::SHADER) {
				asset = new asset::shader_asset(name);
			}
			else if (type == AssetType::MATERIAL) {
				asset = new asset::MaterialAsset(name);
			}
			else if (type == AssetType::MESH) {
				asset = new asset::MeshAsset(name);
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
		
		asset::Asset* AssetManager::get_or_read(const std::string& name) {
			if (!has(name)) {
				return read_asset(name);
			}
			else {
				return get_by_name(name);
			}
		}

		void AssetManager::save_asset(asset::Asset* asset) {
			YAML::Emitter out;

			out << YAML::BeginMap;

			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Name" << YAML::Value << asset->getName();
			out << YAML::Key << "Type" << YAML::Value << (size_t)asset->type();

			if (!asset->serialize(out)) {
				LOG_ERROR("Failed to save asset {0}", asset->getName());
				return;
			}

			out << YAML::EndMap;

			std::ofstream fout(get_res_dir() / asset->getName());
			fout << out.c_str();
		}

		void AssetManager::unload_all() {
			// Unload all then delete
			for (auto& m_asset : m_assets) {
				m_asset->unload();
			}

			for (auto& m_asset : m_assets) {
				delete m_asset;
			}

			m_assets.clear();
		}

		void AssetManager::unload(const std::string& name) {
			auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](Asset* asset) {
				return asset->getName() == name;
			});

			if (it == m_assets.end()) {
				return;
			}

			delete (*it);
			m_assets.erase(it);
		}

		bool AssetManager::can_create(const std::string& name) {
			// Check for conflicts (editor only thing so this will never be called from scripts / runtime so no need to check for binary format)
			if (std::filesystem::exists(get_res_dir() / name)) {
				return false;
			}

			return true;
		}
	}
}
