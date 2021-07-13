#pragma once

#include <string>
#include "aderite/interfaces/iserializable.hpp"
#include "aderite/utility/pointer.hpp"

namespace aderite {
	namespace asset {
		using asset_handle = size_t;

		enum class asset_group : size_t
		{
			SYSTEMIC = 0,
			SHADER = 1
		};

		/**
		 * @brief Asset base generic class
		*/
		class asset_base : public interfaces::iserializable {
		public:
			virtual ~asset_base() {}

			std::string get_name() const {
				return p_name;
			}

			void set_name(const std::string& name) {
				p_name = name;
			}

			asset_handle get_handle() const {
				return p_handle;
			}

			/**
			 * @brief Returns true if the asset is in group
			*/
			virtual bool in_group(asset_group group) const = 0;

			/**
			 * @brief Prepare the asset to be loaded into memory
			*/
			virtual void prepare_load() = 0;

			/**
			 * @brief Is the asset ready to be loaded
			 * @return True if the asset can be loaded, false otherwise
			*/
			virtual bool ready_to_load() = 0;

			/**
			 * @brief Load the asset from memory into an actual object, this has no effect
			 * on assets that are not graphic, audio, etc.
			*/
			virtual void load() = 0;

			/**
			 * @brief Unload the asset from memory, but keep the information how to load it
			 * back at any other time
			*/
			virtual void unload() = 0;
		protected:
			asset_base(asset_handle handle)
				: p_handle(handle)
			{}

			friend class asset_manager;
		protected:
			std::string p_name;
			asset_handle p_handle;
			
			// Asset loaded into memory
			bool p_loaded = false;
		};
	}
}
