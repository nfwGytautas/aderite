#pragma once

#include <functional>
#include "aderite/interfaces/iframe_object.hpp"

namespace aderite {

	/**
	 * @brief Generic window class
	*/
	class window : public interfaces::iframe_object {
	public:
		/**
		 * @brief Window create options passed to create_window, most can be changed at runtime
		*/
		struct create_options {
			int width = 1280;
			int height = 720;
			const char* title = "Window";
		};

	public:
		bool closed = false;

	public:
		window() {}
		virtual ~window() {}

		/**
		 * @brief Destroys the window
		*/
		virtual void destroy() = 0;

		/**
		 * @brief Makes this window the current active one where rendering will be submitted to
		*/
		virtual void make_active();
	private:
		window(const window& o) = delete;
		friend class window_manager;
	};

}
