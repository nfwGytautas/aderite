#include "WindowsEditor.hpp"

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3.h>
#include <pfd/portable-file-dialogs.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/window/WindowManager.hpp"

#include "aderiteeditor/compiler/ScriptCompiler.hpp"
#include "aderiteeditor/runtime/Instancers.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/windows/AssetBrowser.hpp"
#include "aderiteeditor/windows/Inspector.hpp"
#include "aderiteeditor/windows/Menubar.hpp"
#include "aderiteeditor/windows/NodeEditor.hpp"
#include "aderiteeditor/windows/SceneHierarchy.hpp"
#include "aderiteeditor/windows/SceneView.hpp"
#include "aderiteeditor/windows/StartupModal.hpp"
#include "aderiteeditor/windows/Toolbar.hpp"
#include "aderiteeditor/windows/backend/bgfx/backend_bgfx.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

#define EVENT_ROUTE(e, dst) event_router::e = std::bind(&WindowsEditor::dst, this, std::placeholders::_1)

namespace aderite {

WindowsEditor* WindowsEditor::m_instance = nullptr;

WindowsEditor::WindowsEditor(int argc, char** argv) {
    StartupModal = new editor_ui::StartupModal();

    Inspector = new editor_ui::Inspector();
    Menubar = new editor_ui::Menubar();
    Toolbar = new editor_ui::Toolbar();
    SceneView = new editor_ui::SceneView();
    SceneHierarchy = new editor_ui::SceneHierarchy();
    AssetBrowser = new editor_ui::AssetBrowser();
    NodeEditor = new editor_ui::NodeEditor();

    editor::State::EditorCamera = new editor::EditorCamera();

    // Setup event router
    editor::State::Sink = this;
    editor::State::Project = nullptr;
    m_instance = this;
}

WindowsEditor::~WindowsEditor() {
    delete Inspector;
    delete Menubar;
    delete Toolbar;
    delete SceneView;
    delete SceneHierarchy;
    delete AssetBrowser;
    delete NodeEditor;

    delete StartupModal;
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

    // Setup instancers for serializer
    utility::linkInstancers();
}

void WindowsEditor::onRendererInitialized() {
    LOG_TRACE("Initializing ImGui UI");

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-SceneView / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    GLFWwindow* handle = static_cast<GLFWwindow*>(::aderite::Engine::get()->getWindowManager()->getImplementationHandle());

    switch (bgfx::getRendererType()) {
    case bgfx::RendererType::OpenGL: {
        ImGui_ImplGlfw_InitForOpenGL(handle, true);
        break;
    }
    case bgfx::RendererType::Vulkan: {
        ImGui_ImplGlfw_InitForVulkan(handle, true);
        break;
    }
    case bgfx::RendererType::Direct3D11: {
        ImGui_ImplGlfw_InitForOther(handle, true);
        break;
    }
    case bgfx::RendererType::Direct3D12: {
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
    StartupModal->init();
    Inspector->init();
    Menubar->init();
    Toolbar->init();
    SceneView->init();
    SceneHierarchy->init();
    AssetBrowser->init();
    NodeEditor->init();

    // Show the startup modal
    StartupModal->show();
}

void WindowsEditor::onStartRender() {}

void WindowsEditor::onPreRenderCommit() {}

void WindowsEditor::onEndRender() {
    // Render ImGui

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
    if (opt_fullscreen) {
        ImGuiViewport* SceneView = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(SceneView->Pos);
        ImGui::SetNextWindowSize(SceneView->Size);
        ImGui::SetNextWindowViewport(SceneView->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask
    // Begin() to not render a background.
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

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("Dockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

    // Dockspace components start here

    this->renderComponents();

    // Dockspace components end here

    ImGui::End();

    // Rendering ImGui
    ImGui::Render();

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
        // onSaveProject();
    }

    StartupModal->shutdown();
    Inspector->shutdown();
    Menubar->shutdown();
    Toolbar->shutdown();
    SceneView->shutdown();
    SceneHierarchy->shutdown();
    AssetBrowser->shutdown();
    NodeEditor->shutdown();

    // Shutdown ImGui
    backend::ImGui_Implbgfx_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();

    delete editor::State::Project;
}

void WindowsEditor::onNewProject(const std::string& dir, const std::string& name) {
    LOG_TRACE("New project name: {0} at directory {1}", name, dir);
    ::aderite::Engine::get()->getWindowManager()->setTitle(name);

    if (editor::State::Project) {
        delete editor::State::Project;
    }

    editor::State::Project = new editor::Project(dir, name);

    // Create directories
    this->createDirectories();

    // Copy shader libs, and aderite editor shaders
    this->copyLibFiles();

    // Setup asset manager
    ::aderite::Engine::getFileHandler()->setRoot(editor::State::Project->getRootDir());

    // Create new scene
    onNewScene("Untitled scene");

    // Save
    editor::State::Project->save();
}

void WindowsEditor::onSaveProject() {
    if (!editor::State::Project) {
        // TODO: Create new project?
        return;
    }

    // Save all assets
    ::aderite::Engine::getSerializer()->saveAll();
    editor::State::Project->save();
}

void WindowsEditor::onLoadProject(const std::string& path) {
    LOG_TRACE("Loading project {0}", path);
    if (editor::State::Project) {
        // TODO: Ask for saving if there are changes
        delete editor::State::Project;
    }

    // Unload all assets
    //::aderite::Engine::getAssetManager()->unloadAll();

    // TODO: Verify all assets are in their name directories

    editor::State::Project = editor::Project::load(path);

    // Copy shader libs, just in case they were updated
    this->copyLibFiles();

    ::aderite::Engine::get()->getWindowManager()->setTitle(editor::State::Project->getName());

    // Setup asset manager
    ::aderite::Engine::getFileHandler()->setRoot(editor::State::Project->getRootDir());

    // Setup audio controller
    ::aderite::Engine::getAudioController()->loadMasterBank();

    // Load assemblies
    compiler::ScriptCompiler sc;
    sc.compile();
    ::aderite::Engine::getScriptManager()->loadAssemblies();

    if (editor::State::Project->getActiveScene() != c_InvalidHandle) {
        // Read scene
        scene::Scene* s =
            static_cast<scene::Scene*>(::aderite::Engine::getSerializer()->getOrRead(editor::State::Project->getActiveScene()));
        ::aderite::Engine::getSceneManager()->setActive(s);
    }
}

void WindowsEditor::onSceneChanged(scene::Scene* scene) {
    // Attach editor camera
    SceneView->onSceneChanged(scene);
}

void WindowsEditor::onSystemUpdate(float delta) {
    editor::State::EditorCamera->update(delta);
}

void WindowsEditor::onPipelineChanged(rendering::Pipeline* pipeline) {}

void WindowsEditor::onNewScene(const std::string& name) {
    LOG_TRACE("New scene with name: {0}", name);

    // TODO: Error screen or special naming
    scene::Scene* s = new scene::Scene();
    ::aderite::Engine::getSerializer()->add(s);
    ::aderite::Engine::getSerializer()->save(s);
    vfs::File* file = new vfs::File(name, s->getHandle(), editor::State::Project->getVfs()->getRoot());

    ::aderite::Engine::getSceneManager()->setActive(s);
}

void WindowsEditor::onStopGame() {
    Engine::get()->stopPhysicsUpdates();
    Engine::get()->stopScriptUpdates();
    Engine::get()->stopSceneUpdates();
    Engine::getAudioController()->disable(true);
    editor::State::IsGameMode = false;

    // TODO: Disable all cameras in scene
}

void WindowsEditor::onStartGame() {
    Engine::get()->startPhysicsUpdates();
    Engine::get()->startScriptUpdates();
    Engine::get()->startSceneUpdates();
    Engine::getAudioController()->disable(false);
    editor::State::IsGameMode = true;

    // TODO: Enable all cameras in scene
}

void WindowsEditor::onResetGameState() {
    // TODO: Reset game state, by reloading all scripts or resetting their default parameters
}

WindowsEditor* WindowsEditor::getInstance() {
    return m_instance;
}

void WindowsEditor::renderComponents() {
    static bool show_demo_window = true;

    // Components
    StartupModal->render();

    if (!StartupModal->isOpen() && editor::State::Project != nullptr) {
        // Only render when open and a project exists
        Inspector->render();
        Menubar->render();
        Toolbar->render();
        SceneView->render();
        SceneHierarchy->render();
        AssetBrowser->render();
        NodeEditor->render();

        // DEMO WINDOW
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }
}

void WindowsEditor::copyLibFiles() {
    const auto copyOptions = std::filesystem::copy_options::overwrite_existing;

    // Shader files
    std::filesystem::copy("res/shaders/include/", editor::State::Project->getRootDir() / "Data/", copyOptions);
    std::filesystem::copy("res/shaders/wireframe/", editor::State::Project->getRootDir() / "Data/", copyOptions);

    // Scriptlib
    std::filesystem::copy("aderite_scriptlib.dll",
                          editor::State::Project->getRootDir() /
                              ("Data/_" + std::to_string(aderite::io::FileHandler::Reserved::ScriptLibCode) + ".data"),
                          copyOptions);
}

void WindowsEditor::createDirectories() {
    if (!std::filesystem::exists(editor::State::Project->getRootDir() / "Asset/")) {
        std::filesystem::create_directory(editor::State::Project->getRootDir() / "Asset/");
    }

    if (!std::filesystem::exists(editor::State::Project->getRootDir() / "Data/")) {
        std::filesystem::create_directory(editor::State::Project->getRootDir() / "Data/");
    }

    if (!std::filesystem::exists(editor::State::Project->getRootDir() / "Scripts/")) {
        std::filesystem::create_directory(editor::State::Project->getRootDir() / "Scripts/");
    }
}

} // namespace aderite
