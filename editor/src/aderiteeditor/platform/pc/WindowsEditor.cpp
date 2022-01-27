#include "WindowsEditor.hpp"

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>
#include <pfd/portable-file-dialogs.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/window/WindowManager.hpp"

#include "aderiteeditor/compiler/ScriptCompiler.hpp"
#include "aderiteeditor/resources/EditorIcons.hpp"
#include "aderiteeditor/runtime/Instancers.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

#define EVENT_ROUTE(e, dst) event_router::e = std::bind(&WindowsEditor::dst, this, std::placeholders::_1)

namespace aderite {

WindowsEditor* WindowsEditor::m_instance = nullptr;

WindowsEditor::WindowsEditor(int argc, char** argv) {
    // Setup event router
    editor::State::Sink = this;
    editor::State::Project = nullptr;
    m_instance = this;
}

WindowsEditor::~WindowsEditor() {}

void WindowsEditor::onRuntimeInitialized() {
    LOG_DEBUG("[Editor] Using WINDOWS editor");

    // Check for pfd
    if (!pfd::settings::available()) {
        LOG_ERROR("[Editor] PFD not available on a WINDOWS editor. Incorrect editor choice? Aborting.");
        ::aderite::Engine::get()->requestExit();
        return;
    }

    // By default disable game updates
    onStopGame();

    // Setup instancers for serializer
    utility::linkInstancers();
}

void WindowsEditor::onRendererInitialized() {
    ADERITE_LOG_BLOCK;

    editor::State::getInstance().init();

    // Load editor icons
    LOG_TRACE("[Editor] Enqueueing editor icon loading");
    if (!editor::EditorIcons::getInstance().load()) {
        LOG_ERROR("[Editor] Couldn't load editor icons");
    }

    // UI
    if (!m_ui.setup()) {
        // Abort
        ::aderite::Engine::get()->requestExit();
    }
}

void WindowsEditor::onStartRender() {}

void WindowsEditor::onPreRenderCommit() {}

void WindowsEditor::onEndRender() {
    // Render UI
    m_ui.render();

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

    // Icons
    editor::EditorIcons::getInstance().unload();

    // UI
    m_ui.shutdown();

    editor::State::getInstance().shutdown();

    delete editor::State::Project;
    editor::State::Project = nullptr;
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

    // Registry
    ::aderite::Engine::getAssetManager()->saveRegistry();

    // Save
    editor::State::Project->save();
}

void WindowsEditor::onSaveProject() {
    if (!editor::State::Project) {
        // TODO: Create new project?
        return;
    }

    // Save all assets
    ::aderite::Engine::getAssetManager()->saveAllTrackedObjects();
    ::aderite::Engine::getAssetManager()->saveRegistry();
    editor::State::Project->save();
}

void WindowsEditor::onLoadProject(const std::string& path) {
    LOG_TRACE("Loading project {0}", path);
    if (editor::State::Project) {
        // TODO: Ask for saving if there are changes
        delete editor::State::Project;
        editor::State::Project = nullptr;
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
    ::aderite::Engine::getAssetManager()->loadRegistry();

    // Setup audio controller
    ::aderite::Engine::getAudioController()->loadMasterBank();

    // Load assemblies
    compiler::ScriptCompiler sc;
    sc.compile();
    ::aderite::Engine::getScriptManager()->loadAssemblies();

    if (editor::State::Project->getActiveScene() != c_InvalidHandle) {
        // Read scene
        scene::Scene* s = static_cast<scene::Scene*>(::aderite::Engine::getAssetManager()->get(editor::State::Project->getActiveScene()));
        ::aderite::Engine::getSceneManager()->setActive(s);
    }
}

void WindowsEditor::onSceneChanged(scene::Scene* scene) {}

void WindowsEditor::onSystemUpdate(float delta) {
    editor::State::getInstance().getEditorCamera()->update(delta);
}

void WindowsEditor::onNewScene(const std::string& name) {
    LOG_TRACE("New scene with name: {0}", name);

    // TODO: Error screen or special naming
    scene::Scene* s = new scene::Scene();
    /*::aderite::Engine::getAssetManager()->track(s);
    ::aderite::Engine::getAssetManager()->save(s);*/
    // vfs::File* file = new vfs::File(name, s->getHandle(), editor::State::Project->getVfs()->getRoot());

    ::aderite::Engine::getSceneManager()->setActive(s);
}

void WindowsEditor::onStopGame() {
    Engine::get()->stopPhysicsUpdates();
    Engine::get()->stopScriptUpdates();
    Engine::getAudioController()->disable(true);
    editor::State::IsGameMode = false;

    // TODO: Disable all cameras in scene
}

void WindowsEditor::onStartGame() {
    Engine::get()->startPhysicsUpdates();
    Engine::get()->startScriptUpdates();
    Engine::getAudioController()->disable(false);
    editor::State::IsGameMode = true;

    Engine::getScriptManager()->onSceneChanged(Engine::getSceneManager()->getCurrentScene());

    // TODO: Enable all cameras in scene
}

void WindowsEditor::onResetGameState() {
    // TODO: Reset game state, by reloading all scripts or resetting their default parameters
}

editor::EditorUI& WindowsEditor::getUI() {
    return m_ui;
}

WindowsEditor* WindowsEditor::getInstance() {
    return m_instance;
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

    // Virtual file system
    if (!std::filesystem::exists(editor::State::Project->getRootDir() / "VFS/")) {
        std::filesystem::create_directory(editor::State::Project->getRootDir() / "VFS/");
    }
}

} // namespace aderite
