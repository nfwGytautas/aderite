#pragma once

#include "aderite/interfaces/iaderite_editor.hpp"
#include "aderite/core/window/window.hpp"
#include "aderite/utility/pointer.hpp"

#include "aderite_editor/project/project.hpp"

// Components forward declare?
#include "aderite_editor/components/toolbar.hpp"
#include "aderite_editor/components/viewport.hpp"
#include "aderite_editor/components/scene_view.hpp"

namespace aderite {
	namespace editor {

		/**
		 * @brief Editor for windows operating system
		*/
		class windows_editor : public interfaces::iaderite_editor {
		public:
			windows_editor(int argc, char** argv);

			virtual void on_runtime_initialized() override;
			virtual void on_renderer_initialized() override;
			virtual void on_end_render() override;
			virtual void on_runtime_shutdown() override;

			// Special
			virtual void on_selected_entity_changed(scene::entity* entity);

			/**
			 * @brief Creates a new project
			 * @param name Name of the project
			*/
			void new_project(const std::string& name);
		private:
			ref<window> m_editor_window = nullptr;
			project* m_project = nullptr;

			// Components
			ref<components::toolbar> m_toolbar = nullptr;
			ref<components::viewport> m_viewport = nullptr;
			ref<components::scene_view> m_scene_view = nullptr;
		};

	}
}
