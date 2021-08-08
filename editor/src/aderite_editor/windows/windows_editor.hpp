#pragma once

#include "aderite/interfaces/iaderite_editor.hpp"
#include "aderite/core/window/window.hpp"
#include "aderite/utility/pointer.hpp"

#include "aderite_editor/core/event_router.hpp"

// Components forward declare?
#include "aderite_editor/components/toolbar.hpp"
#include "aderite_editor/components/viewport.hpp"
#include "aderite_editor/components/scene_view.hpp"
#include "aderite_editor/components/property_editor.hpp"
#include "aderite_editor/components/asset_browser.hpp"

namespace aderite {
	namespace editor {

		/**
		 * @brief Editor for windows operating system
		*/
		class windows_editor : public interfaces::iaderite_editor, public ievent_sink {
		public:
			windows_editor(int argc, char** argv);

			virtual void on_runtime_initialized() override;
			virtual void on_renderer_initialized() override;
			virtual void on_end_render() override;
			virtual void on_runtime_shutdown() override;

			// ============================================================================
			// Start of event routes, look at ievent_sink interface for more information
			// ============================================================================
			virtual void selected_entity_changed(scene::entity& entity) override;
			virtual void new_project(const std::string& dir, const std::string& name) override;
			virtual void save_project() override;
			virtual void load_project(const std::string& path) override;
			virtual void new_scene(const std::string& name) override;
			virtual void create_entity(const std::string& name) override;
			virtual void destroy_entity(const scene::entity& entity) override;
		private:
			ref<window> m_editor_window = nullptr;

			// Components
			ref<components::toolbar> m_toolbar = nullptr;
			ref<components::viewport> m_viewport = nullptr;
			ref<components::scene_view> m_scene_view = nullptr;
			ref<components::property_editor> m_property_editor = nullptr;
			ref<components::asset_browser> m_asset_browser = nullptr;
		};

	}
}
