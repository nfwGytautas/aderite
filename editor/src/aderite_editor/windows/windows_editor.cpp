#include "windows_editor.hpp"

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "aderite/aderite.hpp"
#include "aderite/core/window/glfw_window.hpp"
#include "aderite/core/rendering/layer.hpp"
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#include "aderite/utility/log.hpp"

class game_layer : public aderite::layer {
public:
	virtual void init() override {
		float vertices[] = {
			 0.5f,  0.5f, 0.0f,  // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
		};

		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\n\0";

		m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(m_vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(m_vertexShader);

		m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(m_fragmentShader);

		m_shader = glCreateProgram();
		glAttachShader(m_shader, m_vertexShader);
		glAttachShader(m_shader, m_fragmentShader);
		glLinkProgram(m_shader);

		glDeleteShader(m_vertexShader);
		glDeleteShader(m_fragmentShader);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_initialized = true;
	}

	virtual void render() override {
		renderer->clear();

		glUseProgram(m_shader);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	virtual void shutdown() override {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteProgram(m_shader);
	}

	virtual bool ready() override {
		return m_initialized;
	}

private:
	bool m_initialized = false;

	unsigned int m_vbo = 0;
	unsigned int m_vertexShader = 0;
	unsigned int m_fragmentShader = 0;
	unsigned int m_shader = 0;
	unsigned int VBO, VAO, EBO;
};

namespace aderite {
	namespace editor {

		windows_editor::windows_editor(int argc, char** argv) {
		}

		void windows_editor::on_runtime_initialized() {
			LOG_DEBUG("Using WINDOWS editor");

			// Create window
			m_editor_window = engine::get_window_manager()->create_window({});

			// TODO: Load the default scene or create a new one
			engine::get_renderer()->add_layer<game_layer>();
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
			//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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

			// Setup game
			m_viewport = aderite::fbo::create({ 800, 600 }).as<aderite::render_backend::opengl::gl_fbo>();
			engine::get_renderer()->set_default_target(m_viewport.relay_as<aderite::fbo>());
			/*auto asset = engine::get_asset_manager()->create<aderite::fbo>(m_viewport.as<aderite::fbo>());
			auto systemic_asset = asset->in_group(aderite::asset::asset_group::SYSTEMIC);*/
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

			// Menubar
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{

					if (ImGui::MenuItem("Exit")) {
						aderite::engine::get()->request_exit();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			if (show_demo_window) {
				ImGui::ShowDemoWindow(&show_demo_window);
			}

			// Viewport
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("Viewport");
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			ImVec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			ImGui::Image((ImTextureID)m_viewport->get_ta(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
			//ImGui::PopStyleVar();

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
			// Shutdown ImGui
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}


	}
}
