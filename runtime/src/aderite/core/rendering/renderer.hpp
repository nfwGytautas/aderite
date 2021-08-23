#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/interfaces/iframe_object.hpp"

namespace aderite {
	class window;

	/**
	 * @brief Generic rendering class
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
		 * @brief Resets the renderer output to be the window
		*/
		virtual void reset_output() = 0;

		/**
		 * @brief Switches the target to the default target
		*/
		virtual void output_to_default() = 0;

		/**
		 * @brief Returns true if renderer ready to render
		*/
		virtual bool ready() = 0;

		/**
		 * @brief Returns the final output framebuffer which can then be rendered to texture in editors, call display_frame()
		 * to render the content of the framebuffer to screen
		*/
		bgfx::FrameBufferHandle get_output();

		/**
		 * @brief Renders output framebuffer to screen
		*/
		void display_frame();
	private:
		/**
		 * @brief Creates a instance of the renderer depending on the rendering backend
		*/
		static renderer* create_instance();

	protected:
		renderer() {}
		friend class engine;

		window* m_attached_to = nullptr;
		bgfx::FrameBufferHandle m_output = BGFX_INVALID_HANDLE;
	};

}
