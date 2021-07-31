#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "aderite/core/assets/asset.hpp"
#include "aderite/utility/pointer.hpp"

namespace aderite {
	namespace asset {

		/**
		 * @brief Aderite asset manager
		*/
		class asset_manager {
		public:
			const std::filesystem::path ResDir = "Assets";
			const std::filesystem::path RawDir = ResDir / std::filesystem::path("Raw");
		public:
			/**
			 * @brief Initializes the asset manager
			*/
			bool init();

			/**
			 * @brief Frees all asset manager information
			*/
			void shutdown();

			/**
			 * @brief Sets the root directory of the manager to the specified one
			*/
			void set_root_dir(const std::string& path);

			/**
			 * @brief Returns the resource directory
			*/
			std::filesystem::path get_res_dir() const {
				return (m_rootDir / ResDir);
			}

			/**
			 * @brief Returns the raw directory
			*/
			std::filesystem::path get_raw_dir() const {
				return (m_rootDir / RawDir);
			}

			/**
			 * @brief Create an asset
			 * @tparam T Type of the asset
			*/
			template<class T, class ...Args>
			T* create(Args&&... args) {
				// Create asset
				T* a = new T(std::forward<Args>(args)...);
				m_assets.push_back(static_cast<asset_base*>(a));
				return a;
			}

			/**
			 * @brief Get an asset by name
			*/
			template<class T>
			T* get_by_name(const std::string& name) {
				auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](asset_base* asset) { 
					return asset->get_name() == name;
				});

				if (it == m_assets.end()) {
					LOG_WARN("Tried to get non existing asset with name {0}", name);
					return nullptr;
				}

				return static_cast<T*>(*it);
			}

			/**
			 * @brief Return true if the asset_manager has information or an asset with the specified name
			*/
			bool has(const std::string& name);

			///**
			// * @brief Loads the specified asset info (path is relative to the root assets directory)
			// * this method does not load the asset, just reads its meta information.
			//*/
			//template<class T>
			//T* read_asset(const std::string& path) {
			//	T* a = new T("", {});
			//	if (!a->deserialize((get_res_dir() / path).string())) {
			//		LOG_ERROR("Couldn't deserialize {0}", path);
			//		delete a;
			//		return nullptr;
			//	}
			//
			//	m_assets.push_back(static_cast<asset_base*>(a));
			//	return a;
			//}

			/**
			 * @brief Loads the specified file contents into a string, guaranteed by the asset manager to be
			 * thread safe
			 * @param path Path to file
			 * @return String containing the contents of the file
			*/
			std::string load_txt_file(const std::string& path);

			/**
			 * @brief Reads asset from file (non binary format), this automatically finds the type
			 * @param path Path to asset
			 * @return Asset pointer or nullptr if reading failed
			*/
			asset::asset_base* read_asset(const std::string& path) {
				return nullptr;
			}
		private:
			asset_manager() {}
			friend class engine;

		private:
			std::vector<asset_base*> m_assets;
			
			std::filesystem::path m_rootDir = "";
		};

	}
}
