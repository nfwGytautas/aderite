#include "AssetBrowser.hpp"
#include <functional>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/PrefabAsset.hpp"
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
#include "aderiteeditor/vfs/Directory.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"

namespace aderite {
namespace editor {

AssetBrowser::AssetBrowser() {}

AssetBrowser::~AssetBrowser() {}

void AssetBrowser::renderNavigator() {
    ImGui::Text("Navigator");

    vfs::Directory* root = editor::State::Project->getVfs()->getRoot();

    // Root is always open
    ImGui::SetNextItemOpen(true);
    this->renderDirectoryNode(root);
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

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    if (ImGui::BeginTable("AssetItemBrowserTable", columnCount, flags)) {
        // Context menu
        if (ImGui::BeginPopupContextWindow()) {
            renderAddItemPopup();
            ImGui::EndPopup();
        }

        ImGui::TableNextRow();

        // Render directories first
        for (vfs::Directory* dir : m_currentDir->getDirectories()) {
            ImGui::TableNextColumn();

            if (this->renderIconButton("folder", cellSize, cellSize)) {
            }

            // Context menu
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete")) {
                    // TODO: Confirmation window
                    editor::State::Project->getVfs()->remove(dir);
                }

                ImGui::EndPopup();
            }

            this->directoryDragDropHandler(dir);
            DragDrop::renderDirectorySource(dir);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                m_currentDir = dir;
            }

            std::string path = dir->getPath();
            if (renamer.renderUI(std::hash<std::string>()(dir->getPath()), dir->getName())) {
                editor::State::Project->getVfs()->rename(dir, renamer.getValue());
            }
        }

        // Files
        for (vfs::File* file : m_currentDir->getFiles()) {
            io::SerializableObject* object = ::aderite::Engine::getSerializer()->getOrRead(file->getHandle());
            bgfx::TextureHandle icon = BGFX_INVALID_HANDLE;

            ImGui::TableNextColumn();

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
                icon = static_cast<asset::TextureAsset*>(object)->getTextureHandle();
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
            case reflection::RuntimeTypes::PREFAB: {
                icon = editor::EditorIcons::getInstance().getIcon("prefab");
                break;
            }
            }

            if (this->renderImageButton(icon, cellSize, cellSize)) {
            }

            // Context menu
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete")) {
                    // TODO: Confirmation window
                    // TODO: Unload from assets, etc.
                    editor::State::Project->getVfs()->remove(file);
                }

                ImGui::EndPopup();
            }

            // Source
            DragDrop::renderSource(object);
            //DragDrop::renderFileSource(file);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (static_cast<reflection::RuntimeTypes>(object->getType()) == reflection::RuntimeTypes::SCENE) {
                    ::aderite::Engine::getSceneManager()->setActive(static_cast<scene::Scene*>(object));
                }

                editor::State::LastSelectedObject = editor::SelectableObject(object);
            }

            if (renamer.renderUI(object->getHandle(), file->getName())) {
                editor::State::Project->getVfs()->rename(file, renamer.getValue());
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

void AssetBrowser::renderAddItemPopup() {
    std::string newName = "";
    io::SerializableObject* object = nullptr;

    if (ImGui::MenuItem("New folder")) {
        m_currentDir->createDirectory("New folder");
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
        ::aderite::Engine::getSerializer()->add(object);
        ::aderite::Engine::getSerializer()->save(object);
        vfs::File* file = new vfs::File(newName, object->getHandle(), m_currentDir);
    }
}

void AssetBrowser::renderDirectoryNode(vfs::Directory* dir) {
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                           ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags node_flags = base_flags;
    if (dir == m_currentDir) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (dir->getDirectories().size() == 0) {
        node_flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool open = ImGui::TreeNodeEx(dir->getName().c_str(), node_flags);
    if (ImGui::IsItemClicked()) {
        m_currentDir = dir;
    }

    this->directoryDragDropHandler(dir);
    DragDrop::renderDirectorySource(dir);

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            // TODO: Confirmation window
            editor::State::Project->getVfs()->remove(dir);
        }

        ImGui::EndPopup();
    }

    if (open) {
        for (vfs::Directory* subDir : dir->getDirectories()) {
            this->renderDirectoryNode(subDir);
        }

        ImGui::TreePop();
    }
}

void AssetBrowser::directoryDragDropHandler(vfs::Directory* dir) {
    vfs::File* file = DragDrop::renderFileTarget();
    if (file != nullptr) {
        // TODO: Message that already exists

        // Get asset from manager and move it to the new directory
        editor::State::Project->getVfs()->move(dir, file);
    }

    vfs::Directory* payloadDir = DragDrop::renderDirectoryTarget();
    if (payloadDir != nullptr) {
        editor::State::Project->getVfs()->move(dir, payloadDir);
    }
}

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

    if (m_currentDir == nullptr) {
        m_currentDir = editor::State::Project->getVfs()->getRoot();
    }

    // Display

    // Navigator
    if (ImGui::BeginTable("AssetBrowserTable", 2, 0)) {
        scene::Entity* entity = static_cast<scene::Entity*>(
            DragDrop::renderTarget(static_cast<aderite::reflection::Type>(aderite::reflection::RuntimeTypes::ENTITY)));
        if (entity != nullptr) {
            // Create prefab
            asset::PrefabAsset* prefab = new asset::PrefabAsset();
            prefab->setPrototype(entity);

            ::aderite::Engine::getSerializer()->add(prefab);
            ::aderite::Engine::getSerializer()->save(prefab);
            vfs::File* file = new vfs::File(entity->getName() + "_prefab", prefab->getHandle(), m_currentDir);
        }

        ImGui::TableSetupColumn("Navigator", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_None);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        renderNavigator();
        ImGui::TableSetColumnIndex(1);
        renderItems();
        ImGui::EndTable();
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
