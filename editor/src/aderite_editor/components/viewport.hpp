#pragma once

#include <bgfx/bgfx.h>
#include "aderite_editor/components/base_component.hpp"

namespace aderite {
	class fbo;

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
				bgfx::FrameBufferHandle m_fbh;
				bgfx::TextureHandle m_fbth;
			};

		}
	}
}