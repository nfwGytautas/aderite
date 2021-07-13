#include "asset_manager.hpp"

#include <fstream>
#include <filesystem>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"

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
			LOG_TRACE("Creating resource directories");

			// Create directories if they don't exist
			if (!std::filesystem::is_directory(RootDir) || !std::filesystem::exists(RootDir)) {
				std::filesystem::create_directory(RootDir);
			}
			
			if (!std::filesystem::is_directory(RawDir) || !std::filesystem::exists(RawDir)) {
				std::filesystem::create_directory(RawDir);
			}
#endif

			return true;
		}

		void asset_manager::shutdown() {
			for (auto& m_asset : m_assets) {
				delete m_asset;
			}

			m_assets.clear();
		}

		asset_handle asset_manager::get_handle() {
			return m_next_handle++;
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

			fin.open(RawDir + path);

			if (!fin)
			{
				LOG_ERROR("Couldn't open the file {0}", path);
				return "";
			}

			fstream << fin.rdbuf();
			fin.close();

			return fstream.str();
		}

	}
}
