#pragma once

#include "aderite/interfaces/iaderite_editor.hpp"
#include "aderite/core/window/window.hpp"

#include "aderite/utility/pointer.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"

namespace aderite {
	namespace editor {

		/**
		 * @brief Editor for windows operating system
		*/
		class windows_editor : public interfaces::iaderite_editor {
		public:
			windows_editor(int argc, char** argv);

			virtual void on_runtime_initialized() override;
			virtual void on_renderer_initialized() override;
			virtual void on_end_render() override;
			virtual void on_runtime_shutdown() override;
		private:
			ref<window> m_editor_window = nullptr;

			// TODO: Move to a UI component
			ref<aderite::render_backend::opengl::gl_fbo> m_viewport;
		};

	}
}
