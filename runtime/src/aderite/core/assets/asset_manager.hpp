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
		 * @brief Aderite asset manager. 
		 * Primary task of the asset manager is as the name implies to handle assets. Important note of the asset manager that it 
		 * works with assets that is known by it. Known assets are those that have been read. If an asset is freed it is no longer 
		 * known by the manager and handles become invalid.
		 * Secondary task is to provide threaded loading operations for various raw data formats.
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
			T* create(const std::string& name, Args&&... args) {
				// Check for conflict
				if (!can_create(name)) {
					return nullptr;
				}

				// Create asset
				T* a = new T(name, std::forward<Args>(args)...);

				// Register
				m_assets.push_back(static_cast<asset_base*>(a));
				return a;
			}

			/**
			 * @brief Returns the asset with the specified name
			 * @param name Name of the asset to get
			 * @return Asset base instance or nullptr if the asset is not known
			*/
			asset_base* get_by_name(const std::string& name);

			/**
			 * @brief Return true if the asset_manager has information or an asset with the specified name
			*/
			bool has(const std::string& name);

			/**
			 * @brief Loads the specified file contents into a string, guaranteed by the asset manager to be
			 * thread safe
			 * @param path Path to file
			 * @return String containing the contents of the file
			*/
			std::string load_txt_file(const std::string& path);

			/**
			 * @brief Reads asset from file (non binary format), this automatically finds the type and makes the asset
			 * known by the manager.
			 * @param path Path to asset
			 * @return Asset pointer or nullptr if reading failed
			*/
			asset::asset_base* read_asset(const std::string& path);

			/**
			 * @brief Gets an asset of the specified name or reads it if it's not known
			 * @param name Name of the asset
			 * @return Asset pointer or nullptr if reading failed
			*/
			asset::asset_base* get_or_read(const std::string& name);

			/**
			 * @brief Saves the asset to device memory
			 * @param asset Asset to save
			*/
			void save_asset(asset::asset_base* asset);

			/**
			 * @brief Unloads all assets
			*/
			void unload_all();

			/**
			 * @brief Removes an asset with the specified name
			 * @param name Name of the asset to remove
			*/
			void unload(const std::string& name);

			auto begin() {
				return m_assets.begin();
			}

			auto begin() const {
				return m_assets.begin();
			}

			auto end() {
				return m_assets.end();
			}

			auto end() const {
				return m_assets.end();
			}
		private:
			asset_manager() {}
			friend class engine;

			bool can_create(const std::string& name);
		private:
			std::vector<asset_base*> m_assets;
			std::filesystem::path m_rootDir = "";
		};

	}
}
