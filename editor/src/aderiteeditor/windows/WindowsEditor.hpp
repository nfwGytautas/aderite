#pragma once

#include "aderite/interfaces/IEngineMiddleware.hpp"
#include "aderite/window/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
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
	virtual void onEndRender() override;
	virtual void onRuntimeShutdown() override;

	// ============================================================================
	// Start of event routes, look at IEventSink interface for more information
	// ============================================================================
	virtual void onSelectedEntityChanged(scene::Entity& Entity) override;
	virtual void onNewProject(const std::string& dir, const std::string& name) override;
	virtual void onSaveProject() override;
	virtual void onLoadProject(const std::string& path) override;
	virtual void onNewScene(const std::string& name) override;
	virtual void onCreateEntity(const std::string& name) override;
	virtual void onDestroyEntity(const scene::Entity& Entity) override;
	virtual void onSelectedAssetChanged(asset::Asset* asset) override;
private:
	window::Window* m_editorWindow = nullptr;

	// Components
	component::Toolbar* m_toolbar = nullptr;
	component::Viewport* m_viewport = nullptr;
	component::SceneView* m_sceneView = nullptr;
	component::EntityEditor* m_propertyEditor = nullptr;
	component::AssetBrowser* m_assetBrowser = nullptr;
	component::AssetEditor* m_assetEditor = nullptr;

	/**
	 * @brief Flag that is true when the user requests to exit the editor otherwise the
	 * application is being closed unexpectedly
	*/
	bool m_expected_shutdown = false;
};

ADERITE_EDITOR_ROOT_NAMESPACE_END
