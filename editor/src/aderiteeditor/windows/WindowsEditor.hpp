#pragma once

#include "aderite/interfaces/IEngineMiddleware.hpp"
#include "aderite/window/Forward.hpp"

#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/windows/Forward.hpp"

namespace aderite {

/**
 * @brief Editor for windows operating system
 */
class WindowsEditor : public interfaces::IEngineMiddleware, public editor::IEventSink {
public:
    WindowsEditor(int argc, char** argv);
    ~WindowsEditor();

    virtual void onRuntimeInitialized() override;
    virtual void onRendererInitialized() override;
    virtual void onStartRender() override;
    virtual void onPreRenderCommit() override;
    virtual void onEndRender() override;
    virtual void onRuntimeShutdown() override;
    virtual void onSceneChanged(scene::Scene* scene) override;
    virtual void onSystemUpdate(float delta) override;
    virtual void onPipelineChanged(rendering::Pipeline* pipeline) override;

    // ============================================================================
    // Start of event routes, look at IEventSink interface for more information
    // ============================================================================
    virtual void onNewProject(const std::string& dir, const std::string& name) override;
    virtual void onSaveProject() override;
    virtual void onLoadProject(const std::string& path) override;
    virtual void onNewScene(const std::string& name) override;
    virtual void onStopGame() override;
    virtual void onStartGame() override;
    virtual void onResetGameState() override;

    // ============================================================================
    // Start of UI components API
    // ============================================================================
    editor_ui::Inspector* Inspector = nullptr;
    editor_ui::Menubar* Menubar = nullptr;
    editor_ui::SceneView* SceneView = nullptr;
    editor_ui::SceneHierarchy* SceneHierarchy = nullptr;
    editor_ui::AssetBrowser* AssetBrowser = nullptr;
    editor_ui::Toolbar* Toolbar = nullptr;
    editor_ui::NodeEditor* NodeEditor = nullptr;
    editor_ui::StartupModal* StartupModal = nullptr;

    static WindowsEditor* getInstance();

private:
    void renderComponents();
    void copyLibFiles();
    void createDirectories();

private:
    /**
     * @brief Flag that is true when the user requests to exit the editor otherwise the
     * application is being closed unexpectedly
     */
    bool m_expected_shutdown = false;

    static WindowsEditor* m_instance;
};

} // namespace aderite
