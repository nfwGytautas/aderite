#include "Menubar.hpp"
#include <filesystem>
#include <vector>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"

#include "aderiteeditor/compiler/ScriptCompiler.hpp"
#include "aderiteeditor/platform/pc/modals/FileDialog.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

// TEMPORARY
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"

namespace aderite {
namespace editor {

Menubar::Menubar() {}

Menubar::~Menubar() {}

bool Menubar::init() {
    return true;
}

void Menubar::shutdown() {}

void Menubar::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New project")) {
                // TODO: Implement
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
                ::aderite::Engine::get()->setState(Engine::CurrentState::AWAITING_SHUTDOWN);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Project")) {
            if (ImGui::MenuItem("Link FMOD project")) {
                // Required name and directory
                std::string dir = FileDialog::selectDirectory();
                editor::State::Project->linkFmodProject(dir);
            }

            if (ImGui::MenuItem("Reload FMOD audio banks")) {
                auto fmodProject = editor::State::Project->getFmodProjectDir();
                if (std::filesystem::exists(fmodProject)) {
                    // Copy master and strings bank
                    const auto copyOptions = std::filesystem::copy_options::overwrite_existing;

                    // Copy files
                    std::filesystem::copy(fmodProject, editor::State::Project->getRootDir() / "Data/", copyOptions);

                    // Rename master and strings
                    std::filesystem::rename(editor::State::Project->getRootDir() / "Data/Master.bank",
                                            aderite::Engine::getFileHandler()->pathToReserved(io::FileHandler::Reserved::MasterAudioBank));
                    std::filesystem::rename(editor::State::Project->getRootDir() / "Data/Master.strings.bank",
                                            aderite::Engine::getFileHandler()->pathToReserved(io::FileHandler::Reserved::StringsAudioBank));

                    // Tell aderite audio engine to reload them
                    ::aderite::Engine::getAudioController()->loadMasterBank();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scripting")) {
            if (ImGui::MenuItem("Load game code")) {
                // Select the code file
                std::string file = FileDialog::selectFile("Select game code", {"Game code", "*.dll"});

                if (!file.empty()) {
                    const std::filesystem::path codeChunkPath =
                        ::aderite::Engine::getFileHandler()->pathToReserved(io::FileHandler::Reserved::GameCode);

                    if (std::filesystem::exists(codeChunkPath)) {
                        std::filesystem::remove(codeChunkPath);
                    }

                    std::filesystem::copy(file, codeChunkPath);

                    // Reload
                    LOG_TRACE("Reloading scripts");
                    ::aderite::Engine::getScriptManager()->loadAssemblies();
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

} // namespace editor
} // namespace aderite
