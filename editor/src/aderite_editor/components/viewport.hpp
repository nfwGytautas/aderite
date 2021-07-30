#pragma once

#include "aderite_editor/components/base_component.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Viewport of the editor, this is the window where the game is actually rendered to
			*/
			class viewport : public base_component {
			public:
				// Inherited via base_component
				virtual void init() override;
				virtual void shutdown() override;
				virtual void render() override;

			private:
				aderite::render_backend::opengl::gl_fbo* m_viewport;
			};

		}
	}
}