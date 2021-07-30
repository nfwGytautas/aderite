#include "windows_editor.hpp"

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"

// Probably not necessary
#include "aderite/core/window/glfw_window.hpp"
#include "aderite/core/rendering/layer.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#include "aderite/core/rendering/shader/shader.hpp"
#include "aderite/core/assets/object/shader_asset.hpp"
#include "aderite/core/scene/scene.hpp"

//class game_layer : public aderite::layer {
//public:
//	virtual void init() override {
//		float vertices[] = {
//			 0.5f,  0.5f, 0.0f,  // top right
//			 0.5f, -0.5f, 0.0f,  // bottom right
//			-0.5f, -0.5f, 0.0f,  // bottom left
//			-0.5f,  0.5f, 0.0f   // top left 
//		};
//
//		unsigned int indices[] = {  // note that we start from 0!
//			0, 1, 3,  // first Triangle
//			1, 2, 3   // second Triangle
//		};
//
//		/*m_shader = aderite::engine::get_asset_manager()->create<aderite::asset::shader_asset>(aderite::asset::shader_asset::fields{
//					"0_vertex.txt",
//					"0_fragment.txt"
//			});
//		m_shader->set_name("QuadShader");
//		m_shader->serialize("res/shaders/0_QuadShader.shader");*/
//
//		aderite::engine::get_asset_manager()->read_asset<aderite::asset::shader_asset>("shaders/0_QuadShader.shader");
//		m_shader = aderite::engine::get_asset_manager()->get_by_name<aderite::asset::shader_asset>("QuadShader");
//		m_shader->prepare_load();
//		m_shader->load();
//
//		aderite::scene::scene* scene = aderite::engine::get_scene_manager()->new_scene();
//		scene->set_name("SampleScene");
//		scene->use_asset(m_shader);
//		scene->serialize("res/scenes/SampleScene.scene");
//		aderite::engine::get_scene_manager()->set_active(scene);
//
//		glGenVertexArrays(1, &VAO);
//		glGenBuffers(1, &VBO);
//		glGenBuffers(1, &EBO);
//		glBindVertexArray(VAO);
//
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindVertexArray(0);
//
//		m_initialized = true;
//	}
//
//	virtual void render() override {
//		renderer->clear();
//
//		(*m_shader)->bind();
//		glBindVertexArray(VAO);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	}
//
//	virtual void shutdown() override {
//		glDeleteVertexArrays(1, &VAO);
//		glDeleteBuffers(1, &VBO);
//		glDeleteBuffers(1, &EBO);
//
//		m_shader = nullptr;
//	}
//
//	virtual bool ready() override {
//		return m_initialized;
//	}
//
//private:
//	bool m_initialized = false;
//
//	unsigned int m_vbo = 0;
//	unsigned int VBO, VAO, EBO;
//
//	aderite::asset::shader_asset* m_shader = nullptr;
//};

namespace aderite {
	namespace editor {

		windows_editor::windows_editor(int argc, char** argv) {
			m_toolbar = new components::toolbar();
			m_viewport = new components::viewport();
			m_scene_view = new components::scene_view();
		}

		void windows_editor::on_runtime_initialized() {
			LOG_DEBUG("Using WINDOWS editor");

			// Create window
			m_editor_window = engine::get_window_manager()->create_window({});

			// No project path until a new one isn't created

			// Startup with an empty project
			new_project("untitled project");

			// TODO: Load the default scene or create a new one
			//engine::get_renderer()->add_layer<game_layer>();
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
			GLFWwindow* handle = aderite::engine::get()->get_window_manager()->get_current_active_window()
				.as<aderite::window_backend::glfw::glfw_window>()->get_handle();

			ImGui_ImplGlfw_InitForOpenGL(handle, true);
			ImGui_ImplOpenGL3_Init("#version 150");

			// Components
			m_viewport->init();
		}

		void windows_editor::on_end_render() {
			// Render ImGui

			// State
			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
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
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Exit
			auto activeWindow = aderite::engine::get()->get_window_manager()->get_current_active_window();
			if (activeWindow.valid() && activeWindow->closed) {
				engine::get()->request_exit();
			}
		}

		void windows_editor::on_runtime_shutdown() {
			m_viewport->shutdown();

			// Shutdown ImGui
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			delete m_project;
		}

		void windows_editor::on_selected_entity_changed(scene::entity* entity) {
			m_scene_view->set_active_entity(entity);
		}

		void windows_editor::new_project(const std::string& name) {
			m_editor_window->set_title(name);
			
			if (m_project) {
				delete m_project;
			}

			m_project = new project();
		}

	}
}
