#include "AssetBrowser.hpp"

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
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/vfs/Directory.hpp"
#include "aderiteeditor/vfs/File.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Recusively collects all files from directory into a vector of paths, pushing paths relative to one point
*/
void recursiveFsCollect(std::vector<std::filesystem::path>& entries, std::filesystem::path dir, std::filesystem::path& relativeTo) {
	for (auto& dir_entry : std::filesystem::directory_iterator(dir)) {
		const auto& it = dir_entry.path();

		if (std::filesystem::is_directory(it)) {
			recursiveFsCollect(entries, it, relativeTo);
		}
		else {
			entries.push_back(std::filesystem::relative(it, relativeTo));
		}
	}
}

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
	if (ImGui::BeginTable("AssetBrowserTable", 2)) {
		ImGui::TableSetupColumn("Navigator", ImGuiTableColumnFlags_WidthFixed, 200.0f);
		ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_None);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		renderNavigator();
		ImGui::TableSetColumnIndex(1);
		renderItems();
		ImGui::EndTable();
	}

	
	//// Back button
	//if (ImGui::Button("<", ImVec2(0.0f , c_status_button_height))) {
	//	m_currentDir = m_currentDir->getParent();
	//}

	//ImGui::SameLine(0.0f, 0.0f);
	//ImGui::Text("|");
	//ImGui::SameLine(0.0f, 0.5f);

	//// Path bar
	//
	//// The root
	//if (ImGui::Button("/", ImVec2(0.0f, c_status_button_height))) {
	//	m_currentDir = editor::State::Project->getVfs()->getRoot();
	//}

	//if (ImGui::BeginDragDropTarget()) {
	//	targetHandler("");
	//	ImGui::EndDragDropTarget();
	//}

	

	/*std::filesystem::path namePath = m_currentDir->getPath();
	std::filesystem::path it_path = "";
	for (std::vector<std::string>::reverse_iterator i = namePath.rbegin(); i != namePath.rend(); ++i) {
		it_path = it_path / *i;
		
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::Button((*i + "/").c_str(), ImVec2(0.0f, c_status_button_height))) {
			m_currentDir = it_path;
			resolveFs();
			break;
		}

		if (ImGui::BeginDragDropTarget()) {
			if (targetHandler(it_path)) {
				resolveFs();
			}
			ImGui::EndDragDropTarget();
		}
	}*/

	ImGui::End();
}

void AssetBrowser::renderNavigator() {
	ImGui::Text("Navigator");
}

void AssetBrowser::renderItems() {
	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	static float cellSize = thumbnailSize + padding;
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY;
	static utility::InlineRename renamer;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);

	if (ImGui::BeginTable("AssetBrowserTable", columnCount, flags)) {
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
					//editor::State::Project->getVfs()->remove(dir);
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__GenericAsset)) {
					const char* asset_name = static_cast<const char*>(payload->Data);

					// Get asset from manager and move it to the new directory
					asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName(asset_name);
					if (asset) {
						std::string oldPath = asset->getName();
						std::string newPath = (path / std::filesystem::path(asset->getName()).filename()).string();

						if (newPath == asset->getName()) {
							return false;
						}

						if (::aderite::Engine::getAssetManager()->has(newPath)) {
							LOG_WARN("Asset {0} already exists", newPath);
							return false;
						}

						asset->setName(newPath);

						if (asset->type() == asset::AssetType::SCENE) {
							if (shared::State::Project->getActiveScene() == asset_name) {
								shared::State::Sink->onSaveProject();
							}
						}

						// Move file
						std::filesystem::rename(
							::aderite::Engine::getAssetManager()->getResDir() / oldPath,
							::aderite::Engine::getAssetManager()->getResDir() / newPath);

					}
				}

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__Directory)) {
					const char* dir = static_cast<const char*>(payload->Data);

					// TODO: Move entire directory and all it's assets
					LOG_WARN("Directory moving is not implemented!");
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload(shared::DDPayloadID__Directory, dir, sizeof(dir));
				ImGui::EndDragDropSource();
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
					//editor::State::Project->getVfs()->remove(dir);
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
				default: {
					target = shared::DDPayloadID__GenericAsset;
				}
				}

				if (!target.empty()) {
					ImGui::SetDragDropPayload(target.c_str(), file, sizeof(file));
				}

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

				switch (static_cast<io::RuntimeSerializables>(object->getType())) {
				case io::RuntimeSerializables::SCENE:
				{
					// TODO: Error check
					// TODO: Move to editor?
					/*scene::Scene* scene = static_cast<scene::Scene*>(::aderite::Engine::getAssetManager()->getOrRead(node.Name));
					::aderite::Engine::getSceneManager()->setActive(scene);*/
					break;
				}
				case io::RuntimeSerializables::MATERIAL:
				case io::RuntimeSerializables::MESH:
				case io::RuntimeSerializables::TEXTURE:
				case io::RuntimeSerializables::MAT_TYPE:
				{
					editor::State::LastSelectedObject = editor::SelectableObject(object);
					break;
				}
				}
			}

			if (current_renaming != node.Name) {
				if (ImGui::Selectable(node.Stem.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						rename_value = node.Stem;
						current_renaming = node.Name;
						appearing = true;
					}
				}
			}
			else {
				if (DynamicInputText("##rename", &rename_value, edit_flags)) {
					current_renaming = "";

					if (!rename_value.empty()) {
						renameItem(node.Name, rename_value + node.Extension);
						resolveFs();
						break;
					}
				}

				if (!ImGui::IsItemActive() && !appearing) {
					current_renaming = "";
				}

				if (appearing) {
					ImGui::SetKeyboardFocusHere();
					appearing = false;
				}
			}
		}

		ImGui::EndTable();
	}
}

void AssetBrowser::renderAddItemPopup() {
	if (ImGui::MenuItem("New folder")) {
		m_currentDir->createDirectory("New folder");
	}

	ImGui::Separator();

	if (ImGui::BeginMenu("New asset")) {
		if (ImGui::MenuItem("Scene")) {
			//createItem<scene::Scene>(m_currentDir, "New scene");
		}

		if (ImGui::MenuItem("Mesh")) {
			//createItem<asset::MeshAsset>(m_currentDir, "New mesh");
		}

		if (ImGui::MenuItem("Material")) {
			//createItem<asset::MaterialAsset>(m_currentDir, "New material");
		}

		if (ImGui::MenuItem("Texture")) {
			//createItem<asset::TextureAsset>(m_currentDir, "New texture");
		}

		if (ImGui::MenuItem("Material type")) {
			//createItem<asset::MaterialTypeAsset>(m_currentDir, "New material type");
		}

		ImGui::EndMenu();
	}
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
