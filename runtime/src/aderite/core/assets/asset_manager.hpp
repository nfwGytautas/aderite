#pragma once

#include <vector>
#include <algorithm>
#include "aderite/core/assets/asset.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/core/assets/asset_template_detail.hpp"

namespace aderite {
	namespace asset {

		/**
		 * @brief Aderite asset manager
		*/
		class asset_manager {
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
			 * @param object Object instance of the asset
			*/
			template<class T>
			ref<asset<T>> create(typename std::enable_if<true, ref<T>>::type object) {
				// Create asset
				ref<asset<T>> a = new asset<T>(object, get_handle());
				m_assets.push_back(a.as<asset_base>());
				return a;
			}

			/**
			 * @brief Get an asset by name
			*/
			template<class T>
			ref<asset<T>> get(const std::string& name) {
				auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](const ref<asset_base>& asset) { 
					return asset->get_name() == name;
				});

				if (it == m_assets.end()) {
					LOG_WARN("Tried to get non existing asset with name {0}", name);
					return nullptr;
				}

				return (*it).as<asset<T>>();
			}

			/**
			 * @brief Get an asset by handle
			*/
			template<class T>
			ref<asset<T>> get(const asset_handle& handle) {
				auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](const ref<asset_base>& asset) {
					return asset->get_handle() == handle;
				});

				if (it == m_assets.end()) {
					LOG_WARN("Tried to get non existing asset with handle {0}", handle);
					return nullptr;
				}

				return (*it).as<asset<T>>();
			}
		private:
			asset_handle get_handle();

		private:
			asset_manager() {}
			friend class engine;

		private:
			size_t m_next_handle = 0;
			std::vector<ref<asset_base>> m_assets;
		};

	}
}
