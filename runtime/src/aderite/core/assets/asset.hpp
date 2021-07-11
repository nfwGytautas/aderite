#pragma once

#include <string>
#include "aderite/utility/pointer.hpp"
#include "aderite/core/assets/asset_template_detail.hpp"

namespace aderite {
	namespace asset {
		using asset_handle = size_t;

		/**
		 * @brief Asset base generic class
		*/
		class asset_base {
		public:
			virtual ~asset_base() {}

			std::string get_name() const {
				return m_name;
			}

			void set_name(const std::string& name) {
				m_name = name;
			}

			asset_handle get_handle() const {
				return m_handle;
			}

			asset_type get_type() const {
				return m_info.type;
			}

			asset_group_set get_group_set() const {
				return m_info.group;
			}

			/**
			 * @brief Returns true if the asset is in group
			*/
			bool in_group(asset_group group) const {
				return m_info.group & (asset_group_set)group;
			}
		protected:
			asset_base(asset_handle handle, const detail::asset_info& info)
				: m_handle(handle), m_info(info)
			{}

			friend class asset_manager;
		private:
			std::string m_name;
			asset_handle m_handle;
			detail::asset_info m_info;
		};

		/**
		 * @brief A public accessor for assets, that automatically casts the underlying object 
		 * @tparam T 
		*/
		template<class T>
		class asset : public asset_base {
		public:
			virtual ~asset() {}

			/**
			 * Member access operator
			 * @return Underlying object const pointer
			 */
			T* operator->() const {
				return m_object.operator->;
			}
		private:
			asset(ref<T> object, asset_handle handle)
				: asset_base(handle, detail::get_asset_info<T>()), m_object(object)
			{}

			friend class asset_manager;
		private:
			ref<T> m_object = nullptr;
		};
	}
}
