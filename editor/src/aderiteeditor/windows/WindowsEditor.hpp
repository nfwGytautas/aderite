#pragma once

#include "aderite/interfaces/IEngineMiddleware.hpp"
#include "aderite/window/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"

ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN

/**
 * @brief Editor for windows operating system
*/
class WindowsEditor : public interfaces::IEngineMiddleware, public shared::IEventSink {
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
	component::Menubar* Menubar = nullptr;
	component::SceneView* SceneView = nullptr;
	component::SceneHierarchy* SceneHierarchy = nullptr;
	component::AssetBrowser* AssetBrowser = nullptr;
	component::Toolbar* Toolbar = nullptr;
	component::NodeEditor* NodeEditor = nullptr;
	editor_ui::StartupModal* StartupModal = nullptr;

	static WindowsEditor* getInstance();
private:
	void renderComponents();
private:
	/**
	 * @brief Flag that is true when the user requests to exit the editor otherwise the
	 * application is being closed unexpectedly
	*/
	bool m_expected_shutdown = false;

	static WindowsEditor* m_instance;
};

ADERITE_EDITOR_ROOT_NAMESPACE_END
