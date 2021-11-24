#pragma once

#include "aderiteeditor/platform/pc/UIComponent.hpp"
#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Asset browser used by aderite. This component allows the user to browse the root asset manager directory.
 */
class AssetBrowser : public UIComponent {
public:
    AssetBrowser();
    virtual ~AssetBrowser();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    void renderNavigator();
    void renderItems();

    void renderAddItemPopup();

    void renderDirectoryNode(vfs::Directory* dir);

    void directoryDragDropHandler(vfs::Directory* dir);

private:
    vfs::Directory* m_currentDir = nullptr;
};

} // namespace editor
} // namespace aderite
