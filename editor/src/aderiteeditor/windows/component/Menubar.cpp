#include "Menubar.hpp"

#include <vector>
#include <filesystem>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/windows/component/FileDialog.hpp"
#include "aderiteeditor/windows/component/Modals.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

Menubar::Menubar() {
	m_textModal = new TextInputModal();
}

Menubar::~Menubar() {
	delete m_textModal;
}

void Menubar::render() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New project")) {
				// Required name and directory
				std::string project_dir = FileDialog::selectDirectory();

				if (!project_dir.empty()) {
					// Now get the name
					m_textModal->setTitle("New project");
					m_textModal->setText("Project name:");
					m_textModal->setConfirmAction([&, project_dir](const std::string& value) {
						shared::State::Sink->onNewProject(project_dir, value);
					});

					m_textModal->show();
				}
			}

			if (ImGui::MenuItem("Save project")) {
				shared::State::Sink->onSaveProject();
			}

			if (ImGui::MenuItem("Load project")) {
				std::string file = FileDialog::selectFile("Select aderite project", { "Aderite project", "*.aproj" });

				if (!file.empty()) {
					shared::State::Sink->onLoadProject(file);
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				::aderite::Engine::get()->requestExit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project")) {
			if (ImGui::MenuItem("New scene")) {
				m_textModal->setTitle("New scene");
				m_textModal->setText("Scene name:");
				m_textModal->setConfirmAction([&](const std::string& value) {
					shared::State::Sink->onNewScene(value);
				});

				m_textModal->show();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Optimize Raw folder")) {
				// Removes unused files from Raw folder
				std::vector<std::filesystem::path> used = {};
				for (asset::Asset* asset : *::aderite::Engine::getAssetManager()) {
					if (asset->isInGroup(asset::AssetGroup::DEPENDS_ON_RAW)) {
						switch (asset->type()) {
						case asset::AssetType::MESH: {
							used.push_back(::aderite::Engine::getAssetManager()->getRawDir() / static_cast<asset::MeshAsset*>(asset)->getFields().SourceFile);
							break;
						}
						case asset::AssetType::SHADER: {
							used.push_back(::aderite::Engine::getAssetManager()->getRawDir() / static_cast<asset::ShaderAsset*>(asset)->getFields().VertexPath);
							used.push_back(::aderite::Engine::getAssetManager()->getRawDir() / static_cast<asset::ShaderAsset*>(asset)->getFields().FragmentPath);
							break;
						}
						case asset::AssetType::SCENE:
						case asset::AssetType::MATERIAL: {
							continue;
						}
						default:
							LOG_WARN("Unimplemented type for DEPENDS_ON_RAW asset, aborting");
							return;
						}
					}
				}

				for (auto& file : std::filesystem::recursive_directory_iterator(::aderite::Engine::getAssetManager()->getRawDir())) {
					auto& it = std::find(used.begin(), used.end(), file.path());

					if (it == used.end()) {
						LOG_TRACE("Removing {0}", file.path().string());
						std::filesystem::remove(file.path());
					}
				}
			}

			if (ImGui::MenuItem("Recompile shaders")) {
				// Get all currently used shaders and then complete a unload/load cycle
				LOG_WARN("Not implemented feature called 'Recompile shaders'");
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	m_textModal->render();
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
