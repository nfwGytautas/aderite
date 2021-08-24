#pragma once

#include "aderite/interfaces/iaderite_editor.hpp"
#include "aderite_editor/core/event_router.hpp"

namespace aderite {
	class window;

	namespace editor {
		namespace components {
			class toolbar;
			class viewport;
			class scene_view;
			class entity_editor;
			class asset_browser;
			class asset_editor;
		}

		/**
		 * @brief Editor for windows operating system
		*/
		class windows_editor : public interfaces::iaderite_editor, public ievent_sink {
		public:
			windows_editor(int argc, char** argv);
			~windows_editor();

			virtual void on_runtime_initialized() override;
			virtual void on_renderer_initialized() override;
			virtual void on_end_render() override;
			virtual void on_runtime_shutdown() override;

			// ============================================================================
			// Start of event routes, look at ievent_sink interface for more information
			// ============================================================================
			virtual void selected_entity_changed(scene::Entity& Entity) override;
			virtual void new_project(const std::string& dir, const std::string& name) override;
			virtual void save_project() override;
			virtual void load_project(const std::string& path) override;
			virtual void new_scene(const std::string& name) override;
			virtual void create_entity(const std::string& name) override;
			virtual void destroy_entity(const scene::Entity& Entity) override;
			virtual void selected_asset_changed(asset::Asset* asset) override;
		private:
			window* m_editor_window = nullptr;

			// Components
			components::toolbar* m_toolbar = nullptr;
			components::viewport* m_viewport = nullptr;
			components::scene_view* m_scene_view = nullptr;
			components::entity_editor* m_property_editor = nullptr;
			components::asset_browser* m_asset_browser = nullptr;
			components::asset_editor* m_asset_editor = nullptr;

			/**
			 * @brief Flag that is true when the user requests to exit the editor otherwise the
			 * application is being closed unexpectedly
			*/
			bool m_expected_shutdown = false;
		};

	}
}
