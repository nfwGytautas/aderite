#pragma once

#include <filesystem>

#include "aderite/io/Forward.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"

#include "aderiteeditor/platform/pc/UIComponent.hpp"
#include "aderiteeditor/platform/pc/modals/Forward.hpp"

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
     * @brief Function invoked when files from the system have been drag and dropped onto the editor
     * @param count Number of files
     * @param paths Paths to those files
     */
    void onFilesDropped(int count, const char** paths);

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
     * @brief A routine for removing directory
     */
    void removeDirectory(const std::filesystem::path& directory);

    /**
     * @brief A routine for removing a file
     */
    void removeFile(const std::filesystem::path& file);

    /**
     * @brief Handles the change of the current directory, this will release reference to assets that are held, for example textures,
     * allowing them to unload
     */
    void handleDirectoryChange(const std::filesystem::path& newDirectory);

    /**
     * @brief Handles the renaming of a directory
     */
    void handleDirectoryRename(const std::filesystem::path& directory, const std::string& newName);

    /**
     * @brief Handles the renaming of a file
     */
    void handleFileRename(const std::filesystem::path& file, const std::string& newName, io::SerializableAsset* asset);

    /**
     * @brief Method handles the drag and drop functionality on directories
     */
    void directoryDragDropHandler(const std::filesystem::path& directory);

    /**
     * @brief Import the specified path as an asset
     * @param path File to import
     * @param type Type of the file
     */
    void importAsset(const std::filesystem::path& path, reflection::RuntimeTypes type);

    /**
     * @brief Add asset to the current selected directory
     * @param asset Asset to add
    */
    void addAsset(io::SerializableAsset* asset);

private:
    // Current selected directory
    std::filesystem::path m_currentDirectory = "";
};

} // namespace editor
} // namespace aderite
