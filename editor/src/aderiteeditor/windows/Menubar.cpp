#include "Menubar.hpp"
#include <filesystem>
#include <vector>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/compiler/ScriptCompiler.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/windows/FileDialog.hpp"
#include "aderiteeditor/windows/Modals.hpp"

namespace aderite {
namespace editor_ui {

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
                        editor::State::Sink->onNewProject(project_dir, value);
                    });

                    m_textModal->show();
                }
            }

            if (ImGui::MenuItem("Save project")) {
                editor::State::Sink->onSaveProject();
            }

            if (ImGui::MenuItem("Load project")) {
                std::string file = FileDialog::selectFile("Select aderite project", {"Aderite project", "*.aproj"});

                if (!file.empty()) {
                    editor::State::Sink->onLoadProject(file);
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
                    editor::State::Sink->onNewScene(value);
                });

                m_textModal->show();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Optimize Raw folder")) {
                // Removes unused files from Raw folder
                /*std::vector<std::filesystem::path> used = {};
                for (asset::Asset* asset : *::aderite::Engine::getAssetManager()) {
                    if (asset->isInGroup(asset::AssetGroup::DEPENDS_ON_RAW)) {
                        switch (asset->type()) {
                        case asset::AssetType::MESH: {
                            used.push_back(::aderite::Engine::getAssetManager()->getRawDir() /
                static_cast<asset::MeshAsset*>(asset)->getFields().SourceFile); break;
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
                }*/
            }

            if (ImGui::MenuItem("Recompile shaders")) {
                // Get all currently used shaders and then complete a unload/load cycle
                LOG_WARN("Not implemented feature called 'Recompile shaders'");
            }

            if (ImGui::MenuItem("Reload FMOD audio banks")) {
                //::aderite::Engine::getAudioController()->loadMasterBank(::aderite::Engine::getAssetManager()->getRawDir());

                // for (audio::Bank* bank : *::aderite::Engine::getAudioController()) {
                //	for (audio::AudioEvent* e : *bank) {
                //		if (e->isLoaded()) {
                //			// Unload and load
                //			e->unload();
                //			e->load();
                //		}
                //	}
                //}
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scripting")) {
            if (ImGui::MenuItem("Compile scripts")) {
                // Compile
                LOG_TRACE("Compiling project scripts");
                compiler::ScriptCompiler sc;
                // TODO: Results, errors, warnings, etc.
                sc.compile();

                // Reload
                LOG_TRACE("Reloading scripts");
                ::aderite::Engine::getScriptManager()->loadAssemblies();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_textModal->render();
}

} // namespace editor_ui
} // namespace aderite
