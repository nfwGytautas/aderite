#pragma once

#include "aderite_editor/components/base_component.hpp"
#include "aderite_editor/components/modals.hpp"

namespace aderite {
	namespace asset {
		class Asset;
	}

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
				void set_active_asset(asset::Asset* asset);

			private:
				/**
				 * @brief UI for shader asset
				*/
				void shader_render();

				/**
				 * @brief UI for material asset
				*/
				void material_render();

				/**
				 * @brief UI for mesh asset
				*/
				void mesh_render();
			private:
				asset::Asset* m_selected_asset = nullptr;
			};

		}
	}
}