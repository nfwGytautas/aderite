#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "aderite/core/assets/asset.hpp"
#include "aderite/utility/pointer.hpp"

namespace aderite {
	namespace asset {

		/**
		 * @brief Aderite asset manager
		*/
		class asset_manager {
		public:
			const std::string RootDir = "res/";
			const std::string RawDir = RootDir + "raw/";

		public:
			/**
			 * @brief Initializes the asset manager
			*/
			bool init();

			/**
			 * @brief Frees all asset_manager information
			*/
			void shutdown();

			/**
			 * @brief Create an asset
			 * @tparam T Type of the asset
			*/
			template<class T, class ...Args>
			T* create(Args&&... args) {
				// Create asset
				T* a = new T(get_handle(), std::forward<Args>(args)...);
				m_assets.push_back(static_cast<asset_base*>(a));
				return a;
			}

			/**
			 * @brief Get an asset by name
			*/
			template<class T>
			T* get(const std::string& name) {
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
			 * @brief Get an asset by handle
			*/
			template<class T>
			T* get(const asset_handle& handle) {
				auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](asset_base* asset) {
					return asset->get_handle() == handle;
				});

				if (it == m_assets.end()) {
					LOG_WARN("Tried to get non existing asset with handle {0}", handle);
					return nullptr;
				}

				return (*it).as<asset<T>>();
			}

			/**
			 * @brief Return true if the asset_manager has information or an asset with the specified name
			*/
			bool has(const std::string& name);

			/**
			 * @brief Loads the specified asset info (path is relative to the root assets directory)
			 * this method does not load the asset, just reads its meta information.
			*/
			template<class T>
			T* read_asset(const std::string& path) {
				T* a = new T(0, {});
				if (!a->deserialize(RootDir + path)) {
					LOG_ERROR("Couldn't deserialize {0}", path);
					delete a;
					return nullptr;
				}

				if (a->p_handle > m_next_handle) {
					m_next_handle = a->p_handle + 1;
				}
				m_assets.push_back(static_cast<asset_base*>(a));
				return a;
			}

			/**
			 * @brief Loads the specified file contents into a string, guaranteed by the asset manager to be
			 * thread safe
			 * @param path Path to file
			 * @return String containing the contents of the file
			*/
			std::string load_txt_file(const std::string& path);
		private:
			asset_handle get_handle();

		private:
			asset_manager() {}
			friend class engine;

		private:
			size_t m_next_handle = 0;
			std::vector<asset_base*> m_assets;
		};

	}
}
