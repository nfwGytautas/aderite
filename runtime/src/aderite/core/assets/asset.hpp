#pragma once

#include <string>
#include "aderite/interfaces/iserializable.hpp"
#include "aderite/interfaces/iloadable.hpp"

namespace aderite {
	namespace asset {
		/**
		 * @brief Possible asset groups a single asset can have multiple groups
		*/
		enum class asset_group : size_t {
			// Is the asset a system type asset
			SYSTEMIC = 0,

			// Does the asset depend on some external resource from Raw/ directory
			DEPENDS_ON_RAW = 1,
		};

		/**
		 * @brief The asset type
		*/
		enum class asset_type : size_t {
			SHADER = 0,
			SCENE = 1,
			MATERIAL = 2,
			MESH = 3,
		};

		/**
		 * @brief Asset base generic class
		*/
		class asset_base : public interfaces::iserializable, public interfaces::iloadable {
		public:
			virtual ~asset_base() {}

			std::string get_name() const {
				return p_name;
			}

			void set_name(const std::string& name) {
				p_name = name;
			}

			/**
			 * @brief Returns the asset type
			*/
			virtual asset_type type() const = 0;

			/**
			 * @brief Returns true if the asset is in group
			*/
			virtual bool in_group(asset_group group) const = 0;
		protected:
			asset_base(const std::string& name)
				: p_name(name)
			{}

			friend class asset_manager;
		protected:
			std::string p_name;
		};
	}
}
