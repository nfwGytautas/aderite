#pragma once

#include "aderite/core/scene/entity.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#include "aderite_editor/components/base_component.hpp"
#include "aderite_editor/components/modals.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Scene view is a component that displays all scene entities in a tree
			*/
			class scene_view : public base_component {
			public:
				scene_view();
				virtual ~scene_view();

				// Inherited via base_component
				virtual void render() override;

				/**
				 * @brief Sets the current active entity (used to identify which is active)
				*/
				void set_active_entity(scene::entity& entity);

			private:
				scene::entity m_selected_entity = scene::entity::null();
				modals::text_input* m_text_modal = nullptr;
			};

		}
	}
}