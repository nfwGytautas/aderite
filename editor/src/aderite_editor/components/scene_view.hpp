#pragma once

#include "aderite/core/scene/Entity.hpp"
#include "aderite_editor/components/base_component.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			namespace modals {
				class text_input;
			}

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
				 * @brief Sets the current active Entity (used to identify which is active)
				*/
				void set_active_entity(scene::Entity& Entity);

			private:
				scene::Entity m_selected_entity = scene::Entity::null();
				modals::text_input* m_text_modal = nullptr;
			};

		}
	}
}