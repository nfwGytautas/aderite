#pragma once

#include "aderite/core/scene/Entity.hpp"
#include "aderite_editor/components/base_component.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Entity editor is a component that allows to edit specific selected Entity information
			*/
			class entity_editor : public base_component {
			public:
				entity_editor();
				virtual ~entity_editor();

				// Inherited via base_component
				virtual void render() override;

				/**
				 * @brief Sets the current active Entity
				*/
				void set_active_entity(scene::Entity& Entity);
			private:
				scene::Entity m_selected_entity = scene::Entity::null();
			};

		}
	}
}