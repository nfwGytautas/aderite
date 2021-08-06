#include "asset_manager.hpp"

#include <fstream>
#include <filesystem>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/random.hpp"

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
			for (auto& m_asset : m_assets) {
				delete m_asset;
			}

			m_assets.clear();
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

		asset::asset_base* asset_manager::read_asset(const std::string& path) {
			return nullptr;
		}
	}
}
