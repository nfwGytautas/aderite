#include "AssetBrowser.hpp"

#include <functional>
#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/RuntimeSerializables.hpp"
#include "aderiteeditor/utility/ImGui.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/DragDropObject.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/vfs/Directory.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

AssetBrowser::AssetBrowser()
{}

AssetBrowser::~AssetBrowser()
{}

void AssetBrowser::render(){
	if (!ImGui::Begin("Asset browser")) {
		ImGui::End();
		return;
	}

	if (m_currentDir == nullptr) {
		m_currentDir = editor::State::Project->getVfs()->getRoot();
	}

	// Display

	// Navigator
	if (ImGui::BeginTable("AssetBrowserTable", 2, ImGuiTableFlags_BordersInnerV)) {
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

void AssetBrowser::renderNavigator() {
	ImGui::Text("Navigator");

	vfs::Directory* root = editor::State::Project->getVfs()->getRoot();

	// Root is always open
	ImGui::SetNextItemOpen(true);
	this->renderDirectoryNode(root);
}

void AssetBrowser::renderItems() {
	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	static float cellSize = thumbnailSize + padding;
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY;
	static utility::InlineRename renamer;

	const float panelWidth = ImGui::GetContentRegionAvail().x;
	const int columnCount = (int)(panelWidth / cellSize);

	if (ImGui::BeginTable("AssetItemBrowserTable", columnCount, flags)) {
		// Context menu
		if (ImGui::BeginPopupContextWindow())
		{
			renderAddItemPopup();
			ImGui::EndPopup();
		}

		ImGui::TableNextRow();

		// Render directories first
		for (vfs::Directory* dir : m_currentDir->getDirectories()) {
			ImGui::TableNextColumn();

			if (ImGui::Button(dir->getName().c_str(), ImVec2(cellSize, cellSize))) {

			}

			// Context menu
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete")) {
					// TODO: Confirmation window
					editor::State::Project->getVfs()->remove(dir);
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget()) {
				this->directoryDragDropHandler(dir);
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource()) {
				editor::DragDropObject obj{ dir };
				ImGui::SetDragDropPayload(shared::DDPayloadID__Directory, &obj, sizeof(editor::DragDropObject));
				ImGui::EndDragDropSource();
			}

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
			ImGui::TableNextColumn();
			
			if (ImGui::Button(file->getName().c_str(), ImVec2(cellSize, cellSize))) {

			}

			// Context menu
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete")) {
					// TODO: Confirmation window
					editor::State::Project->getVfs()->remove(file);
				}

				ImGui::EndPopup();
			}

			io::SerializableObject* object = ::aderite::Engine::getSerializer()->getOrRead(file->getHandle());
			if (ImGui::BeginDragDropSource()) {
				// Typed
				std::string target = "";

				switch (static_cast<io::RuntimeSerializables>(object->getType())) {
				case io::RuntimeSerializables::SCENE: {
					target = shared::DDPayloadID__SceneAsset;
					break;
				}
				case io::RuntimeSerializables::MATERIAL: {
					target = shared::DDPayloadID__MaterialAsset;
					break;
				}
				case io::RuntimeSerializables::MESH: {
					target = shared::DDPayloadID__MeshAsset;
					break;
				}
				case io::RuntimeSerializables::TEXTURE: {
					target = shared::DDPayloadID__TextureAsset;
					break;
				}
				case io::RuntimeSerializables::MAT_TYPE: {
					target = shared::DDPayloadID__MaterialType;
					break;
				}
				case io::RuntimeSerializables::PIPELINE: {
					target = shared::DDPayloadID__PipelineAsset;
					break;
				}
				default: {
					target = shared::DDPayloadID__GenericAsset;
				}
				}

				if (!target.empty()) {
					editor::DragDropObject obj{ file };
					ImGui::SetDragDropPayload(target.c_str(), &obj, sizeof(editor::DragDropObject));
				}

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

				if (static_cast<io::RuntimeSerializables>(object->getType()) == io::RuntimeSerializables::SCENE) {
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
			object = new asset::MaterialTypeAsset();
			newName = "New material type";
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
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

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

	if (ImGui::BeginDragDropTarget()) {
		this->directoryDragDropHandler(dir);
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginDragDropSource()) {
		editor::DragDropObject obj{ dir };
		ImGui::SetDragDropPayload(shared::DDPayloadID__Directory, &obj, sizeof(editor::DragDropObject));
		ImGui::EndDragDropSource();
	}

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
	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__GenericAsset)) {
		vfs::File* file = static_cast<vfs::File*>(payload->Data);

		// TODO: Message that already exists

		// Get asset from manager and move it to the new directory
		editor::State::Project->getVfs()->move(dir, file);
	}

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__Directory)) {
		editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
		vfs::Directory* payloadDir = static_cast<vfs::Directory*>(ddo->Data);
		editor::State::Project->getVfs()->move(dir, payloadDir);
	}
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
