#include "AssetBrowser.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/windows/component/ComponentUtility.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_status_button_height = 20.0f;

template<typename T>
T* createItem(const std::filesystem::path& root, const std::string& default_name) {
	std::filesystem::path path_name = root / (default_name);
	int it = 1;
	while (::aderite::Engine::getAssetManager()->has(path_name.string())) {
		path_name = root / (default_name + " " + std::to_string(it));
	}
	T* s = ::aderite::Engine::getAssetManager()->create<T>(path_name.string());
	::aderite::Engine::getAssetManager()->saveAsset(s);
	return s;
}

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

/**
 * @brief Drag and drop handler for asset browser elements
 * @param path New path to move to
 * @return True if object was moved, false otherwise
*/
bool targetHandler(const std::filesystem::path& path) {
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

			return true;
		}
	}

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__Directory)) {
		const char* dir = static_cast<const char*>(payload->Data);

		// TODO: Move entire directory and all it's assets
		LOG_WARN("Directory moving is not implemented!");
	}

	return false;
}

AssetBrowser::AssetBrowser()
{}

AssetBrowser::~AssetBrowser()
{}

void AssetBrowser::render(){
	static int fcnt = 0;
	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY;
	static float cellSize = thumbnailSize + padding;
	static std::string current_renaming = "";
	static bool appearing = false;
	static std::string rename_value = "";
	static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;

	if (!ImGui::Begin("Asset browser")) {
		ImGui::End();
		return;
	}

	auto& res_dir = ::aderite::Engine::getAssetManager()->getResDir();

	if (!std::filesystem::exists(res_dir)) {
		ImGui::End();
		return;
	}

	if (fcnt == m_updateInterval) {
		// Check for directory updates
		resolveFs();
		fcnt = 0;
	}
	else {
		fcnt++;
	}

	// Display
	
	// Back button
	if (ImGui::Button("<", ImVec2(0.0f , c_status_button_height))) {
		m_currentDir = m_currentDir.parent_path();
		resolveFs();
	}
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Text("|");
	ImGui::SameLine(0.0f, 0.5f);

	// Path bar
	
	// The root
	if (ImGui::Button("/", ImVec2(0.0f, c_status_button_height))) {
		m_currentDir = "";
		resolveFs();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (targetHandler("")) {
			resolveFs();
		}
		ImGui::EndDragDropTarget();
	}

	std::filesystem::path it_path = "";
	for (std::vector<std::string>::reverse_iterator i = m_pathNodes.rbegin(); i != m_pathNodes.rend(); ++i) {
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
	}

	ImGui::Separator();

	// Files
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);

	if (ImGui::BeginTable("AssetBrowserTable", columnCount, flags)) {
		// Context menu
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New folder")) {
				std::filesystem::path dir = res_dir / m_currentDir / "New folder";
				int dir_it = 1;
				while (std::filesystem::exists(dir)) {
					dir = res_dir / m_currentDir / ("New folder " + std::to_string(dir_it++));
				}
				std::filesystem::create_directory(dir);
				resolveFs();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("New asset")) {
				if (ImGui::MenuItem("Scene")) {
					createItem<scene::Scene>(m_currentDir, "New scene");
				}

				if (ImGui::MenuItem("Mesh")) {
					createItem<asset::MeshAsset>(m_currentDir, "New mesh");
				}

				if (ImGui::MenuItem("Material")) {
					createItem<asset::MaterialAsset>(m_currentDir, "New material");
				}

				if (ImGui::MenuItem("Shader")) {
					createItem<asset::ShaderAsset>(m_currentDir, "New shader");
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		ImGui::TableNextRow();
		for (FsNode& node : m_itNode) {
			ImGui::TableNextColumn();

			//ImGui::Text("%ld %s %s", node.Type, node.Name.c_str(), node.Stem.c_str());
			if (ImGui::Button(node.Name.c_str(), ImVec2(cellSize, cellSize))) {

			}

			// Context menu
			if (ImGui::BeginPopupContextItem())
			{
				std::filesystem::path path = res_dir / node.Name;
				if (path != ::aderite::Engine::getAssetManager()->getRawDir()) {
					if (ImGui::MenuItem("Delete")) {
						// TODO: Confirmation window
						deleteItem(path);
						resolveFs();
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource()) {
				// Typed
				std::string target = "";
				switch (node.Type) {
				case FsNodeType::SCENE:
				{
					target = shared::DDPayloadID__SceneAsset;
					break;
				}
				case FsNodeType::SHADER:
				{
					target = shared::DDPayloadID__ShaderAsset;
					break;
				}
				case FsNodeType::MATERIAL:
				{
					target = shared::DDPayloadID__MaterialAsset;
					break;
				}
				case FsNodeType::MESH:
				{
					target = shared::DDPayloadID__MeshAsset;
					break;
				}
				case FsNodeType::BANK:
				{
					target = shared::DDPayloadID__AudioBank;
					break;
				}
				case FsNodeType::DIRECTORY:
				{
					target = shared::DDPayloadID__Directory;
					break;
				}
				default:
				{
					target = shared::DDPayloadID__GenericAsset;
				}
				}

				if (!target.empty()) {
					ImGui::SetDragDropPayload(target.c_str(), node.Name.c_str(), node.Name.size() + 1);
				}

				ImGui::EndDragDropSource();
			}

			if (node.Type == FsNodeType::DIRECTORY) {
				if (ImGui::BeginDragDropTarget()) {
					if (targetHandler(node.Name)) {
						resolveFs();
					}
					ImGui::EndDragDropTarget();
				}
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

				switch (node.Type) {
				case FsNodeType::DIRECTORY:
				{
					m_currentDir = m_currentDir / node.Stem;
					resolveFs();
					break;
				}
				case FsNodeType::SCENE:
				{
					// TODO: Error check
					// TODO: Move to editor?
					scene::Scene* scene = static_cast<scene::Scene*>(::aderite::Engine::getAssetManager()->getOrRead(node.Name));
					::aderite::Engine::getSceneManager()->setActive(scene);
					break;
				}
				case FsNodeType::MATERIAL:
				case FsNodeType::SHADER:
				case FsNodeType::MESH:
				{
					shared::State::Sink->onSelectedAssetChanged(::aderite::Engine::getAssetManager()->getOrRead(node.Name));
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

	ImGui::End();
}

void AssetBrowser::resolveFs() {
	// Get all files in the current directory
	auto& res_dir = ::aderite::Engine::getAssetManager()->getResDir();
	m_itNode.clear();
	m_pathNodes.clear();

	// Resolve path nodes
	std::filesystem::path it_path = m_currentDir;
	while (!it_path.empty()) {
		m_pathNodes.push_back(it_path.stem().string());
		it_path = it_path.parent_path();
	}

	for (auto& dir_entry : std::filesystem::directory_iterator(res_dir / m_currentDir)) {
		const auto& name = std::filesystem::relative(dir_entry.path(), res_dir);
		const auto& stem = dir_entry.path().stem().string();
		const auto& ext = dir_entry.path().extension();
		FsNodeType type = FsNodeType::UNKNOWN;

		if (dir_entry.is_directory()) {
			type = FsNodeType::DIRECTORY;
		}
		else {
			// TODO: Error check
			if (ext == ".scene") {
				type = FsNodeType::SCENE;
			}
			else if (ext == ".shader") {
				type = FsNodeType::SHADER;
			}
			else if (ext == ".mesh") {
				type = FsNodeType::MESH;
			}
			else if (ext == ".material") {
				type = FsNodeType::MATERIAL;
			}
			else if (ext == ".bank") {
				type = FsNodeType::BANK;
			}
		}

		// Resolve extension
		m_itNode.push_back({type, stem, name.string(), ext.string()});
	}
}

void AssetBrowser::deleteItem(std::filesystem::path path) {
	if (path == ::aderite::Engine::getAssetManager()->getRawDir()) {
		LOG_WARN("Tried to delete Raw directory");
		return;
	}

	if (std::filesystem::is_directory(path)) {
		// Scan entire folder for assets and delete from asset manager
		for (auto& dir_entry : std::filesystem::directory_iterator(path)) {
			const auto& it = dir_entry.path();
			deleteItem(it);
		}
	}
	else {
		// Remove from asset manager
		// No need to check, cause all assets are read on editors
		::aderite::Engine::getAssetManager()->unload(std::filesystem::relative(path, ::aderite::Engine::getAssetManager()->getResDir()).string());
	}

	// TODO: Check error
	std::filesystem::remove(path);
}

void AssetBrowser::renameItem(const std::string& prevName, const std::string& newName) {
	auto& res_dir = ::aderite::Engine::getAssetManager()->getResDir();
	std::filesystem::path parent = std::filesystem::path(prevName).parent_path();
	std::string fullNewName = (parent / newName).string();

	if (std::filesystem::exists(res_dir / fullNewName)) {
		// Conflict
		return;
	}

	if (std::filesystem::is_directory(res_dir / prevName)) {
		// Recursively rename

		// Collect all paths
		std::vector<std::filesystem::path> entries = {};
		recursiveFsCollect(entries, res_dir / prevName, res_dir / prevName);

		// Rename directory
		std::filesystem::rename(res_dir / prevName, res_dir / fullNewName);

		// Rename all assets
		for (auto& path : entries) {
			asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName((prevName / path).string());
			if (asset != nullptr) {
				asset->setName((fullNewName / path).string());

				// Validate project shared::State::
				shared::State::Project->validate();
			}
		}
	}
	else {
		// This is most likely unnecessary
		if (!::aderite::Engine::getAssetManager()->has(fullNewName)) {
			asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName(prevName);
			if (asset != nullptr) {
				asset->setName(fullNewName);
				
				// Move file
				std::filesystem::rename(res_dir / prevName, res_dir / fullNewName);

				// Validate project shared::State::
				shared::State::Project->validate();
			}
		}
	}
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
