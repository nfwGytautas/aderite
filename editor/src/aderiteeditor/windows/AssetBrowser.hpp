#pragma once

#include <filesystem>

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Asset browser used by aderite. This component allows the user to browse the root asset manager directory.
 */
class AssetBrowser : public editor::BaseComponent {
public:
    AssetBrowser();
    virtual ~AssetBrowser();

    // Inherited via BaseComponent
    virtual void render() override;

private:
    void renderNavigator();
    void renderItems();

    void renderAddItemPopup();

    void renderDirectoryNode(vfs::Directory* dir);

    void directoryDragDropHandler(vfs::Directory* dir);

private:
    vfs::Directory* m_currentDir = nullptr;
};

} // namespace editor_ui
} // namespace aderite
