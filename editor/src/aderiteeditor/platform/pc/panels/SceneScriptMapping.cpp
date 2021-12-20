#include "SceneScriptMapping.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptData.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"

#include "aderiteeditor/platform/pc/WindowsEditor.hpp"
#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"

namespace aderite {
namespace editor {

SceneScriptMapping::SceneScriptMapping() {}

SceneScriptMapping::~SceneScriptMapping() {}

bool SceneScriptMapping::init() {
    return true;
}

void SceneScriptMapping::shutdown() {}

void SceneScriptMapping::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (!ImGui::Begin("Script mapping") || currentScene == nullptr) {
        ImGui::End();
        return;
    }

    scripting::ScriptEventMap* eventMap = currentScene;

    if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_None)) {
        for (const auto& sd : currentScene->getScriptData()) {
            if (ImGui::TreeNode(sd->getScriptName())) {
                // Fields
                if (ImGui::BeginTable((std::string(sd->getScriptName()) + "EditTable").c_str(), 2)) {
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                    ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

                    for (scripting::FieldWrapper fw : sd->getClass()->getFields()) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(fw.getName().c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::PushItemWidth(-FLT_MIN);

                        switch (fw.getType()) {
                        case scripting::FieldType::Float: {
                            float val = fw.getValueType<float>(sd->getInstance());
                            if (ImGui::InputFloat(std::string("#" + fw.getName()).c_str(), &val, NULL)) {
                                fw.setValueType(sd->getInstance(), val);
                            }
                            break;
                        }
                        case scripting::FieldType::Boolean: {
                            bool val = fw.getValueType<bool>(sd->getInstance());
                            if (ImGui::Checkbox(std::string("##" + fw.getName()).c_str(), &val)) {
                                fw.setValueType(sd->getInstance(), val);
                            }
                            break;
                        }
                        case scripting::FieldType::Integer: {
                            int val = fw.getValueType<int>(sd->getInstance());
                            if (ImGui::InputInt(std::string("##" + fw.getName()).c_str(), &val)) {
                                fw.setValueType(sd->getInstance(), val);
                            }
                            break;
                        }
                        case scripting::FieldType::Mesh: {
                            asset::MeshAsset* handle = static_cast<asset::MeshAsset*>(fw.getSerializable(sd->getInstance()));

                            if (handle) {
                                ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            } else {
                                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            }

                            asset::MeshAsset* object = DragDrop::renderTarget<asset::MeshAsset>(reflection::RuntimeTypes::MESH);
                            if (object != nullptr) {
                                fw.setSerializable(sd->getInstance(), object);
                            }

                            break;
                        }
                        case scripting::FieldType::Material: {
                            asset::MaterialAsset* handle = static_cast<asset::MaterialAsset*>(fw.getSerializable(sd->getInstance()));

                            if (handle) {
                                ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            } else {
                                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            }

                            asset::MaterialAsset* object = DragDrop::renderTarget<asset::MaterialAsset>(reflection::RuntimeTypes::MATERIAL);
                            if (object != nullptr) {
                                fw.setSerializable(sd->getInstance(), object);
                            }

                            break;
                        }
                        case scripting::FieldType::Audio: {
                            asset::AudioAsset* handle = static_cast<asset::AudioAsset*>(fw.getSerializable(sd->getInstance()));

                            if (handle) {
                                ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            } else {
                                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            }

                            asset::AudioAsset* object = DragDrop::renderTarget<asset::AudioAsset>(reflection::RuntimeTypes::AUDIO);
                            if (object != nullptr) {
                                fw.setSerializable(sd->getInstance(), object);
                            }

                            break;
                        }
                        /*case scripting::FieldType::AudioSource: {
                            audio::AudioSource* handle = static_cast<audio::AudioSource*>(fw.getSerializable());

                            if (handle) {
                                ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            } else {
                                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                            }

                            audio::AudioSource* object =
                            DragDrop::renderTarget<audio::AudioSource>(reflection::RuntimeTypes::AUDIO_SOURCE); if (object != nullptr)
                            {
                                fw.setSerializable(object);
                            }

                            break;
                        }*/
                        default: {
                            ImGui::Text("Unknown field type");
                        }
                        }

                        ImGui::PopItemWidth();
                    }

                    ImGui::EndTable();
                }

                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_None)) {
        ImGui::PushItemWidth(-FLT_MIN);
        for (scripting::ScriptEvent* sue : eventMap->getEvents(scripting::ScriptEventType::UPDATE)) {
            ImGui::Selectable(sue->getFullName().c_str());
        }
        ImGui::PopItemWidth();

        float width = ImGui::GetContentRegionAvail().x * 0.4855f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
        if (ImGui::Button("Add", ImVec2(width, 0.0f))) {
            WindowsEditor::getInstance()->getUI().pushModal(
                new SelectScriptModal(scripting::ScriptEventType::UPDATE, [](scripting::ScriptEvent* e) {
                    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
                    scripting::ScriptEventMap* eventMap = currentScene;
                    eventMap->add(e);
                }));
        }
    }

    if (ImGui::CollapsingHeader("Scene loaded", ImGuiTreeNodeFlags_None)) {
        ImGui::PushItemWidth(-FLT_MIN);
        for (scripting::ScriptEvent* sue : eventMap->getEvents(scripting::ScriptEventType::SCENE_LOADED)) {
            ImGui::Selectable(sue->getFullName().c_str());
        }
        ImGui::PopItemWidth();

        float width = ImGui::GetContentRegionAvail().x * 0.4855f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
        if (ImGui::Button("Add", ImVec2(width, 0.0f))) {
            WindowsEditor::getInstance()->getUI().pushModal(
                new SelectScriptModal(scripting::ScriptEventType::SCENE_LOADED, [](scripting::ScriptEvent* e) {
                    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
                    scripting::ScriptEventMap* eventMap = currentScene;
                    eventMap->add(e);
                }));
        }
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
