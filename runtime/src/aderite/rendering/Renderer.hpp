#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/window/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

	/**
	 * @brief The Renderer of aderite powered by bgfx
	*/
	class Renderer {
	public:
		virtual ~Renderer() {}

		/**
		 * @brief Initializes the Renderer
		*/
		bool init(window::Window* wnd);

		/**
		 * @brief Shutdown Renderer
		*/
		void shutdown();

		/**
		 * @brief Enables or disables vsync
		*/
		void set_vsync(bool enabled);

		/**
		 * @brief Clears the current output target
		*/
		void clear();

		/**
		 * @brief Resets the Renderer output to be the window
		*/
		void reset_output();

		/**
		 * @brief Executes a single render pass
		*/
		void render();

		/**
		 * @brief Returns true if Renderer ready to render
		*/
		bool isReady();

		// TODO: Rethink this
		void begin_frame();
		void end_frame();

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
		 * @brief Creates a instance of the Renderer depending on the rendering backend
		*/
		static Renderer* create_instance();

	private:
		Renderer() {}
		friend class engine;

	private:
		bool m_isInitialized;
		window::Window* m_attached_to = nullptr;
		bgfx::FrameBufferHandle m_output = BGFX_INVALID_HANDLE;
	};

ADERITE_RENDERING_NAMESPACE_END
