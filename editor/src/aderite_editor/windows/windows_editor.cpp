#include "windows_editor.hpp"

#include <bgfx/bgfx.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3.h>
#include <pfd/portable-file-dialogs.h>

#include "aderite_editor/backends/bgfx/backend_bgfx.hpp"

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/rendering/renderer.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/scene/scene_manager.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/window/window_manager.hpp"
#include "aderite/core/window/window.hpp"
#include "aderite/core/window/glfw_window.hpp"
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/core/project.hpp"
#include "aderite_editor/components/toolbar.hpp"
#include "aderite_editor/components/viewport.hpp"
#include "aderite_editor/components/scene_view.hpp"
#include "aderite_editor/components/entity_editor.hpp"
#include "aderite_editor/components/asset_browser.hpp"
#include "aderite_editor/components/asset_editor.hpp"


#define EVENT_ROUTE(e, dst) event_router::e = std::bind(&windows_editor::dst, this, std::placeholders::_1)

namespace aderite {
	namespace editor {

		windows_editor::windows_editor(int argc, char** argv) {
			m_toolbar = new components::toolbar();
			m_viewport = new components::viewport();
			m_scene_view = new components::scene_view();
			m_property_editor = new components::entity_editor();
			m_asset_browser = new components::asset_browser();
			m_asset_editor = new components::asset_editor();

			// Setup event router
			state::Sink = this;
			state::Project = nullptr;
		}

		windows_editor::~windows_editor() {
			delete m_toolbar;
			delete m_viewport;
			delete m_scene_view;
			delete m_property_editor;
			delete m_asset_browser;
			delete m_asset_editor;
		}

		void windows_editor::on_runtime_initialized() {
			LOG_DEBUG("Using WINDOWS editor");

			// Create window
			m_editor_window = engine::get_window_manager()->create_window({});

			// Check for pfd
			if (!pfd::settings::available()) {
				LOG_ERROR("PFD not available on a WINDOWS editor. Incorrect editor choice? Aborting.");
				engine::get()->request_exit();
				return;
			}

			// Default title
			m_editor_window->set_title("Aderite");

			// TODO: Startup dialog e.g. create new project, load project, etc.
			this->load_project("../example/ExampleProject/ExampleProject.aproj");
		}

		void windows_editor::on_renderer_initialized() {
			LOG_TRACE("Initializing ImGui UI");

			// Initialize ImGui
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
			//io.ConfigViewportsNoAutoMerge = true;
			//io.ConfigViewportsNoTaskBarIcon = true;

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			// Setup Platform/Renderer backends
			GLFWwindow* handle = static_cast<aderite::window_backend::glfw::glfw_window*>(
				aderite::engine::get()->get_window_manager()->get_current_active_window())->get_glfw_window();

			switch (bgfx::getRendererType()) {
			case bgfx::RendererType::OpenGL:
			{
				ImGui_ImplGlfw_InitForOpenGL(handle, true);
				break;
			}
			case bgfx::RendererType::Vulkan:
			{
				ImGui_ImplGlfw_InitForVulkan(handle, true);
				break;
			}
			case bgfx::RendererType::Direct3D11:
			{
				ImGui_ImplGlfw_InitForOther(handle, true);
				break;
			}
			case bgfx::RendererType::Direct3D12:
			{
				ImGui_ImplGlfw_InitForOther(handle, true);
				break;
			}
			default:
				LOG_ERROR("Non supported bgfx renderer type {0} for editor", bgfx::getRendererType());
				engine::get()->request_exit();
				return;
			}
			ImGui_Implbgfx_Init(255);

			// Components
			m_toolbar->init();
			m_viewport->init();
			m_scene_view->init();
			m_property_editor->init();
			m_asset_browser->init();
			m_asset_editor->init();
		}

		void windows_editor::on_end_render() {
			// Render ImGui

			// State
			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			// Start the Dear ImGui frame
			ImGui_Implbgfx_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Note: Switch this to true to enable dockspace
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
				window_flags |= ImGuiWindowFlags_NoBackground;
			}

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Dockspace Window", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen) {
				ImGui::PopStyleVar(2);
			}

			// DockSpace
			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("Dockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			style.WindowMinSize.x = minWinSizeX;

			// Dockspace components start here

			// Components
			m_toolbar->render();
			m_viewport->render();
			m_scene_view->render();
			m_property_editor->render();
			m_asset_browser->render();
			m_asset_editor->render();

			// DEMO WINDOW
			if (show_demo_window) {
				ImGui::ShowDemoWindow(&show_demo_window);
			}

			// Dockspace components end here

			ImGui::End();

			// Rendering ImGui
			ImGui::Render();


			// Reset the renderer output, this binds the renderer window and sets the viewport
			engine::get_renderer()->reset_output();
			// Clear previous output
			engine::get_renderer()->clear();
			// Render to the window
			ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

			// Exit
			auto activeWindow = aderite::engine::get()->get_window_manager()->get_current_active_window();
			if (activeWindow != nullptr && activeWindow->closed) {
				// TODO: Request save
				m_expected_shutdown = true;
				save_project();
				engine::get()->request_exit();
			}
		}

		void windows_editor::on_runtime_shutdown() {
			if (!m_expected_shutdown) {
				// Try to save
				LOG_WARN("Unexpected shutdown trying to save");
				save_project();
			}

			m_toolbar->shutdown();
			m_viewport->shutdown();
			m_scene_view->shutdown();
			m_property_editor->shutdown();
			m_asset_browser->shutdown();
			m_asset_editor->shutdown();

			// Shutdown ImGui
			ImGui_Implbgfx_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			delete state::Project;
		}

		void windows_editor::selected_entity_changed(scene::entity& entity) {
			m_scene_view->set_active_entity(entity);
			m_property_editor->set_active_entity(entity);
		}

		void windows_editor::new_project(const std::string& dir, const std::string& name) {
			LOG_TRACE("New project name: {0} at directory {1}", name, dir);
			m_editor_window->set_title(name);
			
			if (state::Project) {
				delete state::Project;
			}

			state::Project = new project(dir, name);

			// Setup asset manager
			engine::get_asset_manager()->set_root_dir(state::Project->get_root_dir().string());
		}

		void windows_editor::save_project() {
			if (!state::Project) {
				// TODO: Create new project?
				return;
			}

			// Save all assets
			for (asset::asset_base* asset : *engine::get_asset_manager()) {
				engine::get_asset_manager()->save_asset(asset);
			}

			state::Project->save();
		}

		void windows_editor::load_project(const std::string& path) {
			ASSERT_RENDER_THREAD;
			LOG_TRACE("Loading project {0}", path);
			if (state::Project) {
				// TODO: Ask for saving if there are changes
				delete state::Project;
			}

			// Unload all assets
			engine::get_asset_manager()->unload_all();

			// TODO: Verify all assets are in their name directories

			state::Project = project::load(path);

			m_editor_window->set_title(state::Project->get_name());

			// Setup asset manager
			engine::get_asset_manager()->set_root_dir(state::Project->get_root_dir().string());

			// Now load every single asset metadata.
			// This is done to be able to move and rename them. By loading them on demand it overcomplicates other
			// parts of the program. Since asset metadata is pretty small (400 bytes at most) at one time in 4GB of RAM
			// about 20 million of them can be loaded. If someone is using this engine with more assets than that then it's
			// rather surprising considering that this isn't a professional game engine. Also note that all these assets are loaded
			// only in editor configuration in runtime this is optimized out and only those assets that are needed are read.

			for (auto& path : std::filesystem::recursive_directory_iterator(engine::get_asset_manager()->get_res_dir())) {
				// Ignore directories
				if (path.is_directory()) {
					continue;
				}

				// Make sure to ignore Raw directory, cause it contains not assets, but their actual data, since Raw directory is one
				// big directory of data, the check is as simple as checking for parent to not be Raw/
				if (path.path().parent_path() == engine::get_asset_manager()->get_raw_dir()) {
					continue;
				}

				// Now read the asset
				std::filesystem::path p = std::filesystem::relative(path.path(), engine::get_asset_manager()->get_res_dir());

				// get_or_read, because read will log a warning, since some assets might have already been loaded by others
				engine::get_asset_manager()->get_or_read(p.string());
			}
			

			if (!state::Project->get_active_scene().empty()) {
				// Should have been read
				scene::scene* s = static_cast<scene::scene*>(engine::get_asset_manager()->get_by_name(state::Project->get_active_scene()));
				engine::get_scene_manager()->set_active(s);
			}
		}

		void windows_editor::new_scene(const std::string& name) {
			LOG_TRACE("New scene with name: {0}", name);

			// TODO: Error screen or special naming
			scene::scene* s = engine::get_asset_manager()->create<scene::scene>(name);
			engine::get_scene_manager()->set_active(s);
		}

		void windows_editor::create_entity(const std::string& name) {
			scene::scene* s = engine::get_scene_manager()->current_scene();
			s->create_entity(::aderite::scene::components::meta(name));
		}

		void windows_editor::destroy_entity(const scene::entity& entity) {
			scene::scene* s = engine::get_scene_manager()->current_scene();
			s->destroy_entity(entity);
		}

		void windows_editor::selected_asset_changed(asset::asset_base* asset) {
			m_asset_editor->set_active_asset(asset);
		}

	}
}
