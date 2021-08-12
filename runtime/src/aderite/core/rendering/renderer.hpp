#pragma once

#include <vector>
#include "aderite/interfaces/iframe_object.hpp"

namespace aderite {
	class window;
	class fbo;

	/**
	 * @brief Generic rendering class, automatically configures for a backend
	*/
	class renderer : public interfaces::iframe_object {
	public:
		virtual ~renderer() {}

		/**
		 * @brief Initializes the renderer
		*/
		virtual bool init(window* wnd);

		/**
		 * @brief Shutdown renderer
		*/
		virtual void shutdown();

		/**
		 * @brief Enables or disables vsync
		*/
		virtual void set_vsync(bool enabled) = 0;

		/**
		 * @brief Clears the current output target
		*/
		virtual void clear() = 0;

		/**
		 * @brief Executes a single render pass
		*/
		void render();

		/**
		 * @brief Sets the render target, if nullptr then the default target will become active
		 * @param target New render target
		*/
		void set_output(fbo* target);

		/**
		 * @brief Resets the renderer output to be the window
		*/
		virtual void reset_output() = 0;

		/**
		 * @brief Set the default output target, if nullptr then the default will be the window
		 * @param target New default target
		*/
		void set_default_target(fbo* target);

		/**
		 * @brief Switches the target to the default target
		*/
		virtual void output_to_default() = 0;

		/**
		 * @brief Returns true if renderer ready to render
		*/
		virtual bool ready() = 0;
	private:
		/**
		 * @brief Creates a instance of the renderer depending on the rendering backend
		*/
		static renderer* create_instance();

	protected:
		renderer() {}
		friend class engine;

		window* m_attached_to = nullptr;
		fbo* m_default_target = nullptr;
	};

}
