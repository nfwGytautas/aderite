#include "WindowsEditor.hpp"

#include <bgfx/bgfx.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3.h>
#include <pfd/portable-file-dialogs.h>

#include "aderiteeditor/windows/backend/bgfx/backend_bgfx.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/window/WindowManager.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/windows/component/Menubar.hpp"
#include "aderiteeditor/windows/component/Toolbar.hpp"
#include "aderiteeditor/windows/component/SceneView.hpp"
#include "aderiteeditor/windows/component/SceneHierarchy.hpp"
#include "aderiteeditor/windows/component/EntityEditor.hpp"
#include "aderiteeditor/windows/component/AssetBrowser.hpp"
#include "aderiteeditor/windows/component/AssetEditor.hpp"


#define EVENT_ROUTE(e, dst) event_router::e = std::bind(&WindowsEditor::dst, this, std::placeholders::_1)

ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN

WindowsEditor::WindowsEditor(int argc, char** argv) {
	m_menubar = new component::Menubar();
	m_toolbar = new component::Toolbar();
	m_sceneView = new component::SceneView();
	m_sceneHierarchy = new component::SceneHierarchy();
	m_propertyEditor = new component::EntityEditor();
	m_assetBrowser = new component::AssetBrowser();
	m_assetEditor = new component::AssetEditor();

	// Setup event router
	shared::State::Sink = this;
	shared::State::Project = nullptr;
}

WindowsEditor::~WindowsEditor() {
	delete m_menubar;
	delete m_toolbar;
	delete m_sceneView;
	delete m_sceneHierarchy;
	delete m_propertyEditor;
	delete m_assetBrowser;
	delete m_assetEditor;
}

void WindowsEditor::onRuntimeInitialized() {
	LOG_DEBUG("Using WINDOWS editor");

	// Check for pfd
	if (!pfd::settings::available()) {
		LOG_ERROR("PFD not available on a WINDOWS editor. Incorrect editor choice? Aborting.");
		::aderite::Engine::get()->requestExit();
		return;
	}

	// By default disable game updates
	onStopGame();

	// TODO: Startup dialog e.g. create new project, load project, etc.
	this->onLoadProject("../example/ExampleProject/ExampleProject.aproj");
}

void WindowsEditor::onRendererInitialized() {
	LOG_TRACE("Initializing ImGui UI");

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-SceneView / Platform Windows
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
	GLFWwindow* handle = static_cast<GLFWwindow*>(::aderite::Engine::get()->getWindowManager()->getImplementationHandle());

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
		LOG_ERROR("Non supported bgfx Renderer type {0} for editor", bgfx::getRendererType());
		::aderite::Engine::get()->requestExit();
		return;
	}
	backend::ImGui_Implbgfx_Init(255);

	// Components
	m_menubar->init();
	m_toolbar->init();
	m_sceneView->init();
	m_sceneHierarchy->init();
	m_propertyEditor->init();
	m_assetBrowser->init();
	m_assetEditor->init();
}

void WindowsEditor::onEndRender() {
	// Render ImGui

	// State
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame
	backend::ImGui_Implbgfx_NewFrame();
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
		ImGuiViewport* SceneView = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(SceneView->Pos);
		ImGui::SetNextWindowSize(SceneView->Size);
		ImGui::SetNextWindowViewport(SceneView->ID);
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
	m_menubar->render();
	m_toolbar->render();
	m_sceneView->render();
	m_sceneHierarchy->render();
	m_propertyEditor->render();
	m_assetBrowser->render();
	m_assetEditor->render();

	// DEMO WINDOW
	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// Dockspace components end here

	ImGui::End();

	// Rendering ImGui
	ImGui::Render();


	// Reset the Renderer output, this binds the Renderer window and sets the SceneView
	::aderite::Engine::getRenderer()->resetOutput();
	// Clear previous output
	::aderite::Engine::getRenderer()->clear();
	// Render to the window
	backend::ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

	// Exit
	if (::aderite::Engine::get()->getWindowManager()->isClosed()) {
		// TODO: Request save
		m_expected_shutdown = true;
		::aderite::Engine::get()->requestExit();
	}
}

void WindowsEditor::onRuntimeShutdown() {
	if (!m_expected_shutdown) {
		// TODO: Try to save some restore information
		LOG_WARN("Unexpected shutdown trying to save");
		//onSaveProject();
	}

	m_menubar->shutdown();
	m_toolbar->shutdown();
	m_sceneView->shutdown();
	m_sceneHierarchy->shutdown();
	m_propertyEditor->shutdown();
	m_assetBrowser->shutdown();
	m_assetEditor->shutdown();

	// Shutdown ImGui
	backend::ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete shared::State::Project;
}

void WindowsEditor::onSelectedEntityChanged(scene::Entity& Entity) {
	m_sceneHierarchy->setActiveEntity(Entity);
	m_propertyEditor->setActiveEntity(Entity);
}

void WindowsEditor::onNewProject(const std::string& dir, const std::string& name) {
	LOG_TRACE("New project name: {0} at directory {1}", name, dir);
	::aderite::Engine::get()->getWindowManager()->setTitle(name);
	
	if (shared::State::Project) {
		delete shared::State::Project;
	}

	shared::State::Project = new shared::Project(dir, name);

	// Setup asset manager
	::aderite::Engine::getAssetManager()->setRootDir(shared::State::Project->getRootDir().string());
}

void WindowsEditor::onSaveProject() {
	if (!shared::State::Project) {
		// TODO: Create new project?
		return;
	}

	// Save all assets
	for (asset::Asset* asset : *::aderite::Engine::getAssetManager()) {
		::aderite::Engine::getAssetManager()->saveAsset(asset);
	}

	shared::State::Project->save();
}

void WindowsEditor::onLoadProject(const std::string& path) {
	LOG_TRACE("Loading project {0}", path);
	if (shared::State::Project) {
		// TODO: Ask for saving if there are changes
		delete shared::State::Project;
	}

	// Unload all assets
	::aderite::Engine::getAssetManager()->unloadAll();

	// TODO: Verify all assets are in their name directories

	shared::State::Project = shared::Project::load(path);

	::aderite::Engine::get()->getWindowManager()->setTitle(shared::State::Project->getName());

	// Setup asset manager
	::aderite::Engine::getAssetManager()->setRootDir(shared::State::Project->getRootDir().string());

	// Now load every single asset metadata.
	// This is done to be able to move and rename them. By loading them on demand it overcomplicates other
	// parts of the program. Since asset metadata is pretty small (400 bytes at most) at one time in 4GB of RAM
	// about 20 million of them can be loaded. If someone is using this engine with more assets than that then it's
	// rather surprising considering that this isn't a professional game engine. Also note that all these assets are loaded
	// only in editor configuration in runtime this is optimized out and only those assets that are needed are read.
	for (auto& path : std::filesystem::recursive_directory_iterator(::aderite::Engine::getAssetManager()->getResDir())) {
		// Ignore directories
		if (path.is_directory()) {
			continue;
		}

		// Make sure to ignore Raw directory, cause it contains not assets, but their actual data, since Raw directory is one
		// big directory of data, the check is as simple as checking for parent to not be Raw/
		if (path.path().parent_path() == ::aderite::Engine::getAssetManager()->getRawDir()) {
			continue;
		}

		// Now read the asset
		std::filesystem::path p = std::filesystem::relative(path.path(), ::aderite::Engine::getAssetManager()->getResDir());

		// getOrRead, because read will log a warning, since some assets might have already been loaded by others
		::aderite::Engine::getAssetManager()->getOrRead(p.string());
	}
	

	if (!shared::State::Project->getActiveScene().empty()) {
		// Should have been read
		scene::Scene* s = static_cast<scene::Scene*>(::aderite::Engine::getAssetManager()->getByName(shared::State::Project->getActiveScene()));
		::aderite::Engine::getSceneManager()->setActive(s);
	}
}

void WindowsEditor::onSceneChanged(scene::Scene* scene) {
	// Attach editor camera
	shared::State::EditorCamera = createEditorCamera();
	scene->attachCamera(shared::State::EditorCamera);
	m_sceneView->onSceneChanged(scene);
}

void WindowsEditor::onSystemUpdate(float delta) {
	shared::State::EditorCamera->update(delta);
}

void WindowsEditor::onNewScene(const std::string& name) {
	LOG_TRACE("New scene with name: {0}", name);

	// TODO: Error screen or special naming
	scene::Scene* s = ::aderite::Engine::getAssetManager()->create<scene::Scene>(name);
	::aderite::Engine::getSceneManager()->setActive(s);
}

void WindowsEditor::onCreateEntity(const std::string& name) {
	scene::Scene* s = ::aderite::Engine::getSceneManager()->getCurrentScene();
	s->createEntity(::aderite::scene::components::MetaComponent(name));
}

void WindowsEditor::onDestroyEntity(const scene::Entity& Entity) {
	scene::Scene* s = ::aderite::Engine::getSceneManager()->getCurrentScene();
	s->destroyEntity(Entity);
}

void WindowsEditor::onSelectedAssetChanged(asset::Asset* asset) {
	m_assetEditor->setActiveAsset(asset);
}

void WindowsEditor::onStopGame() {
	Engine::get()->stopPhysicsUpdates();
	Engine::get()->stopScriptUpdates();
	Engine::get()->stopSceneUpdates();
	shared::State::IsGameMode = false;

	// TODO: Disable all cameras in scene
}

void WindowsEditor::onStartGame() {
	Engine::get()->startPhysicsUpdates();
	Engine::get()->startScriptUpdates();
	Engine::get()->startSceneUpdates();
	shared::State::IsGameMode = true;

	// TODO: Enable all cameras in scene
}

void WindowsEditor::onResetGameState() {
	// TODO: Reset game state, by reloading all scripts or resetting their default parameters
}

shared::EditorCamera* WindowsEditor::createEditorCamera() {
	return new shared::EditorCamera();
}

ADERITE_EDITOR_ROOT_NAMESPACE_END
