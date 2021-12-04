#include "AssetBrowser.hpp"
#include <functional>
#include <map>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/resources/EditorIcons.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

size_t getSubDirectoryCount(const std::filesystem::path& directory) {
    return static_cast<size_t>(std::distance(std::filesystem::directory_iterator {directory}, std::filesystem::directory_iterator {}));
}

AssetBrowser::AssetBrowser() {}

AssetBrowser::~AssetBrowser() {}

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
        m_currentDirectory = this->getVFSRootAbsolute();
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
            if (entry.is_directory()) {
                // Directory
                ImGui::TableNextColumn();

                if (this->renderIconButton("folder", cellSize, cellSize)) {
                }

                // Context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete")) {
                        this->remove(entry);
                    }

                    ImGui::EndPopup();
                }

                /*this->directoryDragDropHandler(dir);
                DragDrop::renderDirectorySource(dir);*/

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    this->handleDirectoryChange(entry);

                    // Don't continue cause need to rerender
                    break;
                }

                // std::string path = dir->getPath();
                // if (renamer.renderUI(std::hash<std::string>()(dir->getPath()), dir->getName())) {
                //    //editor::State::Project->getVfs()->rename(dir, renamer.getValue());
                //}

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
                        // icon = editor::EditorIcons::getInstance().getIcon("loading");
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
                if (!loadSpinner) {
                    if (this->renderImageButton(icon, cellSize, cellSize)) {
                    }
                } else {
                    utility::ImSpinner("loading_spinner", (thumbnailSize / 4), 12, 2.0);
                }

                // Context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete")) {
                        this->remove(entry);
                    }

                    ImGui::EndPopup();
                }

                // Source
                // DragDrop::renderSource(object);
                // DragDrop::renderFileSource(file);

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    editor::State::LastSelectedObject = editor::SelectableObject(object);
                }

                /*if (renamer.renderUI(object->getHandle(), file->getName())) {
                    editor::State::Project->getVfs()->rename(file, renamer.getValue());
                }*/
            } else {
                // Unknown
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

void AssetBrowser::renderAddItemPopup() {
    std::string newName = "";
    io::SerializableAsset* object = nullptr;

    if (ImGui::MenuItem("New folder")) {
        std::filesystem::create_directory(m_currentDirectory / "New folder");
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Rendering pipeline")) {
        object = new asset::RenderingPipeline();
        newName = "New rendering pipeline";
    }

    if (ImGui::BeginMenu("New asset")) {
        if (ImGui::MenuItem("Scene")) {
            object = new scene::Scene();
            newName = "New scene";
        }

        if (ImGui::MenuItem("Mesh")) {
            object = new asset::MeshAsset();
            newName = "New mesh";
        }

        if (ImGui::MenuItem("Material")) {
            object = new asset::MaterialAsset();
            newName = "New material";
        }

        if (ImGui::MenuItem("Texture")) {
            object = new asset::TextureAsset();
            newName = "New texture";
        }

        if (ImGui::MenuItem("Material type")) {
            object = new asset::EditorMaterialType();
            newName = "New material type";
        }

        if (ImGui::MenuItem("Audio clip")) {
            object = new asset::AudioAsset();
            newName = "New audio clip";
        }

        ImGui::EndMenu();
    }

    if (object != nullptr && !newName.empty()) {
        // TODO: Create symlink
        // TODO: Save asset

        /*::aderite::Engine::getAssetManager()->track(object);
        ::aderite::Engine::getAssetManager()->save(object);
        vfs::File* file = new vfs::File(newName, object->getHandle(), m_currentDir);*/
    }
}

void AssetBrowser::renderNavigatorDirectory(const std::filesystem::path& directory) {
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                           ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags node_flags = base_flags;
    if (directory == m_currentDirectory) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (getSubDirectoryCount(directory) == 0) {
        node_flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool open = ImGui::TreeNodeEx(directory.stem().string().c_str(), node_flags);
    if (ImGui::IsItemClicked()) {
        this->handleDirectoryChange(directory);
    }

    // this->directoryDragDropHandler(dir);
    // DragDrop::renderDirectorySource(dir);

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            this->remove(directory);

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

void AssetBrowser::remove(const std::filesystem::path& entry) {
    // TODO: Confirmation window
    // TODO: Remove symlink and asset
    // TODO: Verify dependencies
    // editor::State::Project->getVfs()->remove(dir);
}

void AssetBrowser::handleDirectoryChange(const std::filesystem::path& newDirectory) {
    m_currentDirectory = newDirectory;
}

// void AssetBrowser::directoryDragDropHandler(vfs::Directory* dir) {
//    vfs::File* file = DragDrop::renderFileTarget();
//    if (file != nullptr) {
//        // TODO: Message that already exists
//
//        // Get asset from manager and move it to the new directory
//        editor::State::Project->getVfs()->move(dir, file);
//    }
//
//    vfs::Directory* payloadDir = DragDrop::renderDirectoryTarget();
//    if (payloadDir != nullptr) {
//        editor::State::Project->getVfs()->move(dir, payloadDir);
//    }
//}

bool AssetBrowser::init() {
    return true;
}

void AssetBrowser::shutdown() {}

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
