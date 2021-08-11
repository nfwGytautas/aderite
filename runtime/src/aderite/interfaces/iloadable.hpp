#pragma once

namespace aderite {
	namespace interfaces {

		/**
		 * @brief Interface for objects that are loadable, these objects are loaded asynchronously by the asset manager so
		 * this object makes sure that everything works neatly
		*/
		class iloadable {
		public:
			virtual ~iloadable() {}

			/**
			 * @brief Prepare the asset to be loaded into memory, this method preferably should not block the caller
			 * therefor it is advised to use asset manager functions to load inside a thread pool
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

			/**
			 * @brief Returns true if the object is being prepared to load, false otherwise
			*/
			virtual bool is_preparing() = 0;

			/**
			 * @brief Returns true if the object is currently loaded into memory, false otherwise
			*/
			virtual bool is_loaded() = 0;
		};

	}
}