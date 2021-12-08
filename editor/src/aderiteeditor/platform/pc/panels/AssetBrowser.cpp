#include "AssetBrowser.hpp"
#include <fstream>
#include <functional>
#include <map>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/platform/pc/EditorUI.hpp"
#include "aderiteeditor/platform/pc/WindowsEditor.hpp"
#include "aderiteeditor/platform/pc/modals/DragDropImportModal.hpp"
#include "aderiteeditor/platform/pc/modals/NotificationModal.hpp"
#include "aderiteeditor/platform/pc/modals/PromptModal.hpp"
#include "aderiteeditor/resources/EditorIcons.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

static AssetBrowser* g_instance = nullptr;

size_t getSubDirectoryCount(const std::filesystem::path& directory) {
    return static_cast<size_t>(std::distance(std::filesystem::directory_iterator {directory}, std::filesystem::directory_iterator {}));
}

AssetBrowser::AssetBrowser() {}

AssetBrowser::~AssetBrowser() {}

void AssetBrowser::onFilesDropped(int count, const char** paths) {
    LOG_TRACE("[Editor] {0} files are being imported from drag drop", count);

    // Iterate over the paths
    for (int i = 0; i < count; i++) {
        // Handle
        const char* path = paths[i];

        // Push import modals
        WindowsEditor::getInstance()->getUI().pushModal(
            new DragDropImportModal(path, std::bind(&AssetBrowser::importAsset, this, std::placeholders::_1, std::placeholders::_2)));
    }
}

void AssetBrowser::renderNavigator() {
    // Header
    ImGui::Text("Navigator");

    // Root is always open
    ImGui::SetNextItemOpen(true);
    this->renderNavigatorDirectory(this->getVFSRootAbsolute());
}

void AssetBrowser::renderItems() {
    static float padding = 8.0f;
    static float thumbnailSize = 64.0f;
    static float cellSize = thumbnailSize + padding;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY;
    static utility::InlineRename renamer;

    const float panelWidth = ImGui::GetContentRegionAvail().x;
    const int columnCount = (int)(panelWidth / cellSize);

    if (columnCount == 0) {
        // On first time the docking will be incorrect
        return;
    }

    if (m_currentDirectory.empty()) {
        this->handleDirectoryChange(this->getVFSRootAbsolute());
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    if (ImGui::BeginTable("AssetItemBrowserTable", columnCount, flags)) {
        // Context menu
        if (ImGui::BeginPopupContextWindow()) {
            renderAddItemPopup();
            ImGui::EndPopup();
        }

        ImGui::TableNextRow();

        for (const auto& entry : std::filesystem::directory_iterator(m_currentDirectory)) {
            const std::string id = entry.path().string();
            const std::string name = entry.path().stem().string();

            if (entry.is_directory()) {
                // Directory
                ImGui::TableNextColumn();

                ImGui::PushID(id.c_str());
                if (this->renderIconButton("folder", cellSize, cellSize)) {
                }

                // Context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete")) {
                        this->removeDirectory(entry);
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    this->handleDirectoryChange(entry);

                    // Don't continue cause need to rerender
                    break;
                }

                this->directoryDragDropHandler(entry);
                DragDrop::renderFileSource(entry);

                // Name
                ImGui::SetWindowFontScale(0.75f);
                if (renamer.renderUI(std::hash<std::string>()(id), name)) {
                    this->handleDirectoryRename(entry, renamer.getValue());
                }
                ImGui::SetWindowFontScale(1.0f);

            } else if (entry.is_regular_file()) {
                // File
                ImGui::TableNextColumn();
                bgfx::TextureHandle icon = BGFX_INVALID_HANDLE;
                bool loadSpinner = false;

                // Get the actual asset this is pointing to, the handle is the extension of the file
                const std::string handleString = entry.path().extension().string().substr(1);
                const io::SerializableHandle handle = std::stoull(handleString);

                // Get it from asset manager
                io::SerializableAsset* object = ::aderite::Engine::getAssetManager()->get(handle);

                // Resolve icon
                switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
                case reflection::RuntimeTypes::SCENE: {
                    icon = editor::EditorIcons::getInstance().getIcon("scene");
                    break;
                }
                case reflection::RuntimeTypes::MATERIAL: {
                    icon = editor::EditorIcons::getInstance().getIcon("material");
                    break;
                }
                case reflection::RuntimeTypes::MESH: {
                    icon = editor::EditorIcons::getInstance().getIcon("mesh");
                    break;
                }
                case reflection::RuntimeTypes::TEXTURE: {
                    auto texture = static_cast<asset::TextureAsset*>(object);

                    // If loaded show texture, otherwise a loading bar
                    if (texture->isValid()) {
                        icon = static_cast<asset::TextureAsset*>(object)->getTextureHandle();
                    } else {
                        // Loading the texture
                        loadSpinner = true;
                    }

                    break;
                }
                case reflection::RuntimeTypes::MAT_TYPE: {
                    icon = editor::EditorIcons::getInstance().getIcon("material_type");
                    break;
                }
                case reflection::RuntimeTypes::PIPELINE: {
                    icon = editor::EditorIcons::getInstance().getIcon("pipeline");
                    break;
                }
                case reflection::RuntimeTypes::AUDIO: {
                    icon = editor::EditorIcons::getInstance().getIcon("audio_clip");
                    break;
                }
                }

                // Render element
                ImGui::PushID(object->getHandle());
                if (!loadSpinner) {
                    if (this->renderImageButton(icon, cellSize, cellSize)) {
                    }
                } else {
                    utility::ImSpinner("loading_spinner", (thumbnailSize / 4), 12, 2.0, glm::vec2(cellSize, cellSize));
                }
                ImGui::PopID();

                // Context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete")) {
                        this->removeFile(entry);
                    }

                    ImGui::EndPopup();
                }

                // Source
                DragDrop::renderAssetFileSource(entry, object);

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    editor::State::getInstance().setSelectedObject(object);
                }

                // Check for outside name rename
                if (name != object->getName()) {
                    // Update the name
                    this->handleFileRename(entry, object->getName(), object);
                } else {
                    ImGui::SetWindowFontScale(0.75f);
                    if (renamer.renderUI(std::hash<std::string>()(id), name)) {
                        this->handleFileRename(entry, renamer.getValue(), object);
                    }
                    ImGui::SetWindowFontScale(1.0f);
                }
            } else {
                // Unknown
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

void AssetBrowser::renderAddItemPopup() {
    io::SerializableAsset* object = nullptr;

    if (ImGui::MenuItem("New folder")) {
        std::filesystem::create_directory(m_currentDirectory / "New folder");
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("New asset")) {
        if (ImGui::MenuItem("Scene")) {
            object = new scene::Scene();
        }

        if (ImGui::MenuItem("Mesh")) {
            object = new asset::MeshAsset();
        }

        if (ImGui::MenuItem("Material")) {
            object = new asset::MaterialAsset();
        }

        if (ImGui::MenuItem("Texture")) {
            object = new asset::TextureAsset();
        }

        if (ImGui::MenuItem("Material type")) {
            object = new asset::EditorMaterialType();
        }

        if (ImGui::MenuItem("Audio clip")) {
            object = new asset::AudioAsset();
        }

        ImGui::EndMenu();
    }

    this->addAsset(object);
}

void AssetBrowser::renderNavigatorDirectory(const std::filesystem::path& directory) {
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags node_flags = base_flags;
    if (directory == m_currentDirectory) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    } else {
        // For subpaths they must be open
        if (std::search(m_currentDirectory.begin(), m_currentDirectory.end(), directory.begin(), directory.end()) !=
            m_currentDirectory.end()) {
            ImGui::SetNextItemOpen(true);
        }
    }

    if (getSubDirectoryCount(directory) == 0) {
        node_flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool open = ImGui::TreeNodeEx(directory.stem().string().c_str(), node_flags);
    if (ImGui::IsItemClicked()) {
        this->handleDirectoryChange(directory);
    }

    this->directoryDragDropHandler(directory);
    DragDrop::renderFileSource(directory);

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            this->removeDirectory(directory);

            ImGui::EndPopup();

            if (open) {
                ImGui::TreePop();
            }

            return;
        }

        ImGui::EndPopup();
    }

    if (open) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                this->renderNavigatorDirectory(entry);
            }
        }

        ImGui::TreePop();
    }
}

std::filesystem::path AssetBrowser::getVFSRootAbsolute() const {
    return editor::State::Project->getRootDir() / "VFS";
}

void AssetBrowser::removeDirectory(const std::filesystem::path& directory) {
    const std::string name = directory.stem().string();

    // Check if empty
    if (!std::filesystem::is_empty(directory)) {
        // Non empty directory can't be removed
        NotificationModal* modal = new NotificationModal("Info", "Can't remove non empty directory, move or delete child items first");
        WindowsEditor::getInstance()->getUI().pushModal(modal);
    } else {
        std::filesystem::remove(directory);
    }
}

void AssetBrowser::removeFile(const std::filesystem::path& file) {
    const std::string name = file.stem().string();

    // Already exist show confirmation window
    PromptModal* modal = new PromptModal("A file or directory already exists do you want to replace it?", "A file by the name of " + name +
                                                                                                              " already exists do you want "
                                                                                                              "to replace it? Doing so "
                                                                                                              "will overwrite its current "
                                                                                                              "contents, "
                                                                                                              "deleting any previous "
                                                                                                              "assets or directories");

    // Add buttons
    modal->pushButton({"Cancel"});
    modal->pushButton({"Delete", [=]() {
                           std::filesystem::remove(file);
                       }});

    // Already exists push a notification window
    WindowsEditor::getInstance()->getUI().pushModal(modal);
}

void AssetBrowser::handleDirectoryChange(const std::filesystem::path& newDirectory) {
    // Acquire references in new directory
    for (const auto& entry : std::filesystem::directory_iterator(newDirectory)) {
        if (entry.is_regular_file()) {
            // Get handle
            const std::string handleString = entry.path().extension().string().substr(1);
            const io::SerializableHandle handle = std::stoull(handleString);

            if (!::aderite::Engine::getAssetManager()->has(handle)) {
                // No longer exists
                std::filesystem::remove(entry);
            } else {
                // Get asset
                io::SerializableAsset* asset = ::aderite::Engine::getAssetManager()->get(handle);

                // Acquire reference
                asset->acquire();
            }
        }
    }

    // Release from previous directory
    this->releaseCurrentDirectoryReferences();

    m_currentDirectory = newDirectory;
}

void AssetBrowser::handleDirectoryRename(const std::filesystem::path& directory, const std::string& newName) {
    const std::filesystem::path newPath = directory.parent_path() / newName;

    if (std::filesystem::exists(newPath)) {
        // Already exist show confirmation window
        PromptModal* modal = new PromptModal("A file or directory already exists do you want to replace it?",
                                             "A file by the name of " + newName +
                                                 " already exists do you want to replace it? Doing so will overwrite its current contents, "
                                                 "deleting any previous assets or directories");

        // Add buttons
        modal->pushButton({"Cancel"});
        modal->pushButton({"Replace", [=]() {
                               std::filesystem::remove(newPath);
                               std::filesystem::rename(directory, newPath);
                           }});

        // Already exists push a notification window
        WindowsEditor::getInstance()->getUI().pushModal(modal);
    } else {
        // Rename
        std::filesystem::remove(newPath);
        std::filesystem::rename(directory, newPath);
    }
}

void AssetBrowser::handleFileRename(const std::filesystem::path& file, const std::string& newName, io::SerializableAsset* asset) {
    const std::filesystem::path newPath = file.parent_path() / (newName + "." + std::to_string(asset->getHandle()));

    // Check if the same name or not
    if (file.stem().string() == newName) {
        return;
    }

    if (std::filesystem::exists(newPath)) {
        // Already exist show confirmation window
        PromptModal* modal = new PromptModal("A file or directory already exists do you want to replace it?",
                                             "A file by the name of " + newName +
                                                 " already exists do you want to replace it? Doing so will overwrite its current contents, "
                                                 "deleting any previous assets or directories");

        // Add buttons
        modal->pushButton({"Cancel"});
        modal->pushButton({"Replace", [=]() {
                               asset->setName(newName);
                               std::filesystem::remove(newPath);
                               std::filesystem::rename(file, newPath);
                               ::aderite::Engine::getAssetManager()->save(asset);
                           }});

        // Already exists push a notification window
        WindowsEditor::getInstance()->getUI().pushModal(modal);
    } else {
        // Rename
        asset->setName(newName);
        std::filesystem::rename(file, newPath);
        ::aderite::Engine::getAssetManager()->save(asset);
    }
}

void AssetBrowser::directoryDragDropHandler(const std::filesystem::path& directory) {
    const std::filesystem::path entry = DragDrop::renderFileTarget();

    if (!entry.empty()) {
        std::filesystem::rename(entry, directory / entry.filename());

        if (entry == m_currentDirectory) {
            m_currentDirectory = directory / entry.filename();
        }
    }
}

void AssetBrowser::importAsset(const std::filesystem::path& path, reflection::RuntimeTypes type) {
    io::SerializableAsset* asset = nullptr;

    switch (type) {
    case reflection::RuntimeTypes::MESH: {
        asset = new asset::MeshAsset();
        break;
    }
    case reflection::RuntimeTypes::TEXTURE: {
        asset = new asset::TextureAsset();
        break;
    }
    default: {
        LOG_ERROR("[Editor] Failed to import {0} cause the type is not implemented", path.string());
    }
    }

    if (asset != nullptr) {
        // Add asset
        this->addAsset(asset);

        // Now copy the source as a loadable id
        ::aderite::Engine::getFileHandler()->writePhysicalFile(asset->getHandle(), path);
    }
}

void AssetBrowser::addAsset(io::SerializableAsset* asset) {
    if (asset != nullptr) {
        ::aderite::Engine::getAssetManager()->track(asset);
        ::aderite::Engine::getAssetManager()->save(asset);
        asset->acquire();

        // Create VFS file
        std::ofstream ofs(m_currentDirectory / (asset->getName() + "." + std::to_string(asset->getHandle())));
    }
}

void AssetBrowser::releaseCurrentDirectoryReferences() {
    if (!m_currentDirectory.empty()) {
        for (const auto& entry : std::filesystem::directory_iterator(m_currentDirectory)) {
            if (entry.is_regular_file()) {
                // Get handle
                const std::string handleString = entry.path().extension().string().substr(1);
                const io::SerializableHandle handle = std::stoull(handleString);

                // Get asset
                io::SerializableAsset* asset = ::aderite::Engine::getAssetManager()->get(handle);

                // Release held reference
                asset->release();
            }
        }
    }
}

bool AssetBrowser::init() {
    ADERITE_DYNAMIC_ASSERT(g_instance == nullptr, "Multiple asset browsers created");

    GLFWwindow* window = static_cast<GLFWwindow*>(aderite::Engine::getWindowManager()->getImplementationHandle());

    g_instance = this;

    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths) {
        g_instance->onFilesDropped(count, paths);
    });

    return true;
}

void AssetBrowser::shutdown() {
    this->releaseCurrentDirectoryReferences();
}

void AssetBrowser::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    if (!ImGui::Begin("Asset browser", nullptr, flags)) {
        ImGui::End();
        return;
    }

    // Display
    if (ImGui::BeginTable("AssetBrowserTable", 2, 0)) {
        // Setup table
        ImGui::TableSetupColumn("Navigator", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_None);
        ImGui::TableNextRow();

        // Navigator
        ImGui::TableSetColumnIndex(0);
        this->renderNavigator();

        // Items
        ImGui::TableSetColumnIndex(1);
        this->renderItems();

        ImGui::EndTable();
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
