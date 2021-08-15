#pragma once

#include <vector>
#include "aderite/interfaces/iframe_object.hpp"
#include "aderite/core/window/window.hpp"

namespace aderite {

	/**
	 * @brief Window manager of aderite engine, usually most applications will only use 1 window
	 * and as such the window manager is built with that
	*/
	class window_manager : public interfaces::iframe_object {
	public:
		/**
		 * @brief Initializes the window manager
		*/
		bool init();

		/**
		 * @brief Shutdown the window manager
		*/
		void shutdown();

		virtual void begin_frame() override;

		virtual void end_frame() override;

		/**
		 * @brief Creates a new window with specified options
		 * @param options Options to create with
		*/
		window* create_window(window::create_options options);

		/**
		 * @brief Function gets called when the current active window is changed by calling window->make_active
		*/
		void active_window_changed(window* window);

		/**
		 * @brief Returns the current active window
		*/
		window* get_current_active_window() {
			return m_current_window;
		}
	private:
		window_manager() {}
		friend class engine;

	private:
		std::vector<window*> m_windows;
		window* m_current_window = nullptr;
	};

}
