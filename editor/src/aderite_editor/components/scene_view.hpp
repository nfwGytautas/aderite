#pragma once

#include "aderite_editor/components/base_component.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#include "aderite/core/scene/entity.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Scene view is a component that displays all scene entities in a tree
			*/
			class scene_view : public base_component {
			public:
				// Inherited via base_component
				virtual void render() override;

				/**
				 * @brief Sets the current active entity (used to identify which is active)
				*/
				void set_active_entity(scene::entity* entity);

			private:
				void render_entity(size_t entity);

			private:
				size_t m_selected_entity = 0;
			};

		}
	}
}