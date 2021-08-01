#pragma once

#include "aderite/core/scene/entity.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#include "aderite_editor/components/base_component.hpp"
#include "aderite_editor/components/modals.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Scene view is a component that allows to edit specific selected entity information
			*/
			class property_editor : public base_component {
			public:
				property_editor();
				virtual ~property_editor();

				// Inherited via base_component
				virtual void render() override;

				/**
				 * @brief Sets the current active entity
				*/
				void set_active_entity(scene::entity& entity);
			private:
				scene::entity m_selected_entity = scene::entity::null();
			};

		}
	}
}