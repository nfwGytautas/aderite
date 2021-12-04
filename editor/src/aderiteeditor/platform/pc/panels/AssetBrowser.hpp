#pragma once

#include <filesystem>

#include "aderiteeditor/platform/pc/UIComponent.hpp"

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
    /**
     * @brief Renders the side navigator for directories
     */
    void renderNavigator();

    /**
     * @brief Renders the items of the current directory
     */
    void renderItems();

    void renderAddItemPopup();

    /**
     * @brief Renders the directory node for the navigator, this is a recursive function
     * @param directory Directory to render
     */
    void renderNavigatorDirectory(const std::filesystem::path& directory);

    /**
     * @brief Returns the current absolute path to the VFS directory
     */
    std::filesystem::path getVFSRootAbsolute() const;

    /**
     * @brief A routine for removing directory or a file
     */
    void remove(const std::filesystem::path& entry);

    /**
     * @brief Handles the change of the current directory, this will release reference to assets that are held, for example textures,
     * allowing them to unload
     */
    void handleDirectoryChange(const std::filesystem::path& newDirectory);

private:
    // Current selected directory
    std::filesystem::path m_currentDirectory = "";
};

} // namespace editor
} // namespace aderite
