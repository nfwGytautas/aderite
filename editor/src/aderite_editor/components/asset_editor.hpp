#pragma once

#include "aderite/core/assets/asset.hpp"
#include "aderite_editor/components/base_component.hpp"
#include "aderite_editor/components/modals.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Asset editor is a component that allows to edit specific selected asset information
			*/
			class asset_editor : public base_component {
			public:
				asset_editor();
				virtual ~asset_editor();

				// Inherited via base_component
				virtual void render() override;

				/**
				 * @brief Sets the current active asset
				*/
				void set_active_asset(asset::asset_base* asset);

			private:
				/**
				 * @brief UI for shader asset
				*/
				void shader_render();

			private:
				asset::asset_base* m_selected_asset = nullptr;
			};

		}
	}
}