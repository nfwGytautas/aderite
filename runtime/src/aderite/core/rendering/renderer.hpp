#pragma once

#include "aderite/interfaces/iframe_object.hpp"

namespace aderite {

	/**
	 * @brief Generic rendering class, automatically configures for a backend
	*/
	class renderer : public interfaces::iframe_object {
	public:
		virtual ~renderer() {}

		/**
		 * @brief Initializes the renderer
		*/
		virtual bool init() = 0;

		/**
		 * @brief Shutdown renderer
		*/
		virtual void shutdown() = 0;

		/**
		 * @brief Enables or disables vsync
		*/
		virtual void set_vsync(bool enabled) = 0;

	private:
		/**
		 * @brief Creates a instance of the renderer depending on the rendering backend
		 * @return 
		*/
		static renderer* create_instance();

	protected:
		renderer() {}
		friend class engine;
	};

}
