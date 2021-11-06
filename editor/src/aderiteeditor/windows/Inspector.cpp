#include "Inspector.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/ColliderListAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Audio.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scene/components/Meta.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/scripting/BehaviorWrapper.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/Script.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/DragDropObject.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/SelectableObject.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"
#include "aderiteeditor/utility/Utility.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/windows/FileDialog.hpp"
#include "aderiteeditor/windows/NodeEditor.hpp"
#include "aderiteeditor/windows/SelectAudioModal.hpp"
#include "aderiteeditor/windows/SelectScriptModal.hpp"
#include "aderiteeditor/windows/WindowsEditor.hpp"

namespace aderite {
namespace editor_ui {

void render_component_shared(const std::string& id, const std::string& label, bool& open, bool& remove) {
    const ImGuiTreeNodeFlags treeNodeFlags = /*ImGuiTreeNodeFlags_DefaultOpen | */ ImGuiTreeNodeFlags_Framed |
                                             ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                             ImGuiTreeNodeFlags_FramePadding;

    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4, 4});
    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

    // ImGui::Separator();

    open = ImGui::TreeNodeEx(id.c_str(), treeNodeFlags, label.c_str());
    ImGui::PopStyleVar();

    ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
    if (ImGui::Button("+", ImVec2 {lineHeight, lineHeight})) {
        ImGui::OpenPopup("ComponentSettings");
    }

    remove = false;
    if (ImGui::BeginPopup("ComponentSettings")) {
        if (ImGui::MenuItem("Remove component")) {
            remove = true;
        }

        ImGui::EndPopup();
    }
}

void Inspector::render() {
    if (!ImGui::Begin("Inspector")) {
        ImGui::End();
        return;
    }

    switch (editor::State::LastSelectedObject.getType()) {
    case editor::SelectableObjectType::Asset: {
        this->renderAsset();
        break;
    }
    case editor::SelectableObjectType::Entity: {
        this->renderEntity();
        break;
    }
    default: {
        ImGui::Text("Select asset or entity");
    }
    }

    ImGui::End();
}

void Inspector::renderEntity() {
    static utility::InlineRename renamer;
    static editor_ui::SelectScriptModal ssm;

    scene::Entity entity = editor::State::LastSelectedObject.getEntity();

    // Component list
    if (!entity.hasComponent<::aderite::scene::MetaComponent>()) {
        // This shouldn't happen
        ImGui::Text("Invalid entity");
        return;
    }

    // Meta
    auto& MetaComponent = entity.getComponent<aderite::scene::MetaComponent>();
    renamer.setValue(MetaComponent.Name);

    if (renamer.renderUI()) {
        MetaComponent.Name = renamer.getValue();
    }

    // Other components as tree nodes that can be collapsed
    ImGui::Separator();

    bool hasTransform = entity.hasComponent<scene::TransformComponent>();
    if (hasTransform) {
        this->renderTransform(entity);
    }

    bool hasMeshRenderer = entity.hasComponent<scene::MeshRendererComponent>();
    if (hasMeshRenderer) {
        this->renderMeshrenderer(entity);
    }

    bool hasDynamicBody = entity.hasComponent<scene::DynamicActor>();
    if (hasDynamicBody) {
        this->renderDynamicBody(entity);
    }

    bool hasStaticBody = entity.hasComponent<scene::StaticActor>();
    if (hasStaticBody) {
        this->renderStaticBody(entity);
    }

    bool hasAudioListener = entity.hasComponent<scene::AudioListenerComponent>();
    if (hasAudioListener) {
        this->renderAudioListener(entity);
    }

    bool hasAudioSource = entity.hasComponent<scene::AudioSourceComponent>();
    if (hasAudioSource) {
        this->renderAudioSource(entity);
    }

    bool hasColliders = entity.hasComponent<scene::CollidersComponent>();
    if (hasColliders) {
        this->renderColliders(entity);
    }

    bool hasScripts = entity.hasComponent<scene::ScriptsComponent>();
    if (hasScripts) {
        this->renderScripts(entity);
    }

    // Add component
    ImGui::Separator();

    float width = ImGui::GetContentRegionAvail().x * 0.4855f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
    if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent")) {
        if (!hasTransform && ImGui::MenuItem("Transform")) {
            entity.addComponent<::aderite::scene::TransformComponent>();
            ImGui::CloseCurrentPopup();
        }

        if (!hasMeshRenderer && ImGui::MenuItem("Mesh Renderer")) {
            entity.addComponent<::aderite::scene::MeshRendererComponent>();
            ImGui::CloseCurrentPopup();
        }

        if (!hasDynamicBody && !hasStaticBody && ImGui::MenuItem("Dynamic body")) {
            entity.addComponent<::aderite::scene::DynamicActor>();
            ImGui::CloseCurrentPopup();
        }

        if (!hasDynamicBody && !hasStaticBody && ImGui::MenuItem("Static body")) {
            entity.addComponent<::aderite::scene::StaticActor>();
            ImGui::CloseCurrentPopup();
        }

        if (!hasAudioListener && ImGui::MenuItem("Audio Listener")) {
            entity.addComponent<::aderite::scene::AudioListenerComponent>();
            ImGui::CloseCurrentPopup();
        }

        if (!hasColliders && ImGui::MenuItem("Collider list")) {
            entity.addComponent<::aderite::scene::CollidersComponent>();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Audio source")) {
            entity.addComponent<::aderite::scene::AudioSourceComponent>();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Script")) {
            ssm.show();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ssm.render();

    if (ssm.getSelectedBehavior() != nullptr) {
        if (!entity.hasComponent<::aderite::scene::ScriptsComponent>()) {
            entity.addComponent<::aderite::scene::ScriptsComponent>();
        }

        // TODO: Block from adding same script again

        // Add script
        auto& scriptsComponent = entity.getComponent<::aderite::scene::ScriptsComponent>();
        scripting::Script* s = new scripting::Script();
        s->setName(ssm.getSelectedBehavior()->getName());
        scriptsComponent.Scripts->addScript(s);

        // Reset the selector
        ssm.reset();
    }
}

void Inspector::renderTransform(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Transform", "Transform", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::TransformComponent>();

        if (utility::DrawVec3Control("Position", c.Position)) {
            if (entity.hasComponent<scene::DynamicActor>()) {
                auto& da = entity.getComponent<scene::DynamicActor>();
                if (da.Actor != nullptr) {
                    da.Actor->moveActor(c.Position);
                }
            } else if (entity.hasComponent<scene::StaticActor>()) {
                auto& sa = entity.getComponent<scene::StaticActor>();
                if (sa.Actor != nullptr) {
                    sa.Actor->moveActor(c.Position);
                }
            }
        }

        glm::vec3 euler = glm::eulerAngles(c.Rotation);
        glm::vec3 rotation = glm::degrees(euler);
        if (utility::DrawVec3Control("Rotation", rotation)) {
            c.Rotation = glm::quat(rotation);

            if (entity.hasComponent<scene::DynamicActor>()) {
                auto& da = entity.getComponent<scene::DynamicActor>();
                if (da.Actor != nullptr) {
                    da.Actor->rotateActor(c.Rotation);
                }
            } else if (entity.hasComponent<scene::StaticActor>()) {
                auto& sa = entity.getComponent<scene::StaticActor>();
                if (sa.Actor != nullptr) {
                    sa.Actor->rotateActor(c.Rotation);
                }
            }
        }

        c.Rotation = glm::radians(rotation);
        if (utility::DrawVec3Control("Scale", c.Scale, 1.0f)) {
            if (entity.hasComponent<scene::CollidersComponent>()) {
                auto& c = entity.getComponent<scene::CollidersComponent>();
                if (c.Colliders != nullptr) {
                    // Force a reattach
                    c.Iteration = c.Iteration--;
                }
            }
        }

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::TransformComponent>();
    }
}

void Inspector::renderMeshrenderer(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Mesh renderer", "Mesh renderer", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::MeshRendererComponent>();

        if (ImGui::BeginTable("MeshRendererTable", 2)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Mesh");

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            if (c.MeshHandle) {
                vfs::File* meshFile = editor::State::Project->getVfs()->getFile(c.MeshHandle->getHandle());
                ImGui::Button(meshFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
            } else {
                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__MeshAsset)) {
                    editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                    vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                    c.MeshHandle = static_cast<asset::MeshAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Material");

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            if (c.MaterialHandle) {
                vfs::File* matFile = editor::State::Project->getVfs()->getFile(c.MaterialHandle->getHandle());
                ImGui::Button(matFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
            } else {
                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__MaterialAsset)) {
                    editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                    vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                    c.MaterialHandle = static_cast<asset::MaterialAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::MeshRendererComponent>();
    }
}

void Inspector::renderDynamicBody(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Dynamic body", "Dynamic body", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::DynamicActor>();

        bool hasGravity = c.Actor->getGravity();
        if (ImGui::Checkbox("Has gravity", &hasGravity)) {
            c.Actor->setGravity(hasGravity);
        }

        bool isKinematic = c.Actor->getKinematic();
        if (ImGui::Checkbox("Is kinematic", &isKinematic)) {
            c.Actor->setKinematic(isKinematic);
        }

        ImGui::Text("Mass");
        ImGui::SameLine();
        float mass = c.Actor->getMass();
        if (ImGui::DragFloat("##X", &mass, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
            c.Actor->setMass(mass);
        }

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::DynamicActor>();
    }
}

void Inspector::renderStaticBody(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Static body", "Static body", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::StaticActor>();

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::StaticActor>();
    }
}

void Inspector::renderAudioListener(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Audio Listener", "Audio Listener", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::AudioListenerComponent>();

        ImGui::Checkbox("Enabled", &c.IsEnabled);

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::AudioListenerComponent>();
    }
}

void Inspector::renderAudioSource(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Audio source", "Audio Source", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::AudioSourceComponent>();

        

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::AudioSourceComponent>();
    }
}

void Inspector::renderColliders(scene::Entity entity) {
    bool open, remove = false;
    render_component_shared("Collider list", "Collider list", open, remove);

    if (open) {
        auto& c = entity.getComponent<scene::CollidersComponent>();

        if (ImGui::BeginTable("ColliderListComponentTable", 2)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("List");

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            if (c.Colliders) {
                vfs::File* colliderFile = editor::State::Project->getVfs()->getFile(c.Colliders->getHandle());
                ImGui::Button(colliderFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
            } else {
                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__ColliderListAsset)) {
                    editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                    vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                    c.Colliders = static_cast<asset::ColliderListAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    if (remove) {
        entity.removeComponent<scene::CollidersComponent>();
    }
}

void Inspector::renderScripts(scene::Entity entity) {
    auto& scripts = entity.getComponent<scene::ScriptsComponent>();
    std::vector<scripting::Script*> toRemove;

    // Colliders
    for (size_t i = 0; i < scripts.Scripts->size(); i++) {
        bool remove = false;

        bool open = false;
        render_component_shared("Script " + std::to_string(i), scripts.Scripts->get(i)->getName(), open, remove);

        if (open) {
            auto script = static_cast<scripting::Script*>(scripts.Scripts->get(i));

            if (ImGui::BeginTable((scripts.Scripts->get(i)->getName() + "EditTable").c_str(), 2)) {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

                // Render fields
                for (scripting::FieldWrapper* fw : script->getBehavior()->getFields()) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(fw->getName().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushItemWidth(-FLT_MIN);

                    switch (fw->getType()) {
                    case scripting::FieldType::Float: {
                        float val = 0.0f;
                        fw->getValue(script->getInstance(), &val);
                        if (ImGui::InputFloat(std::string("#" + fw->getName()).c_str(), &val, NULL)) {
                            fw->setValue(script->getInstance(), &val);
                        }
                        break;
                    }
                    case scripting::FieldType::Boolean: {
                        bool val = false;
                        fw->getValue(script->getInstance(), &val);
                        if (ImGui::Checkbox(std::string("##" + fw->getName()).c_str(), &val)) {
                            fw->setValue(script->getInstance(), &val);
                        }
                        break;
                    }
                    case scripting::FieldType::Mesh: {
                        MonoObject* mesh = fw->getValueObject(script->getInstance());

                        if (mesh) {
                            asset::MeshAsset* meshHandle = nullptr;
                            scripting::extractMesh(mesh, meshHandle);

                            vfs::File* materialFile = editor::State::Project->getVfs()->getFile(meshHandle->getHandle());
                            ImGui::Button(materialFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        } else {
                            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        }

                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__MeshAsset)) {
                                editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                                vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                                asset::MeshAsset* newMeshHandle =
                                    static_cast<asset::MeshAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                                fw->setValue(script->getInstance(), ::aderite::Engine::getScriptManager()->getLocator().create(newMeshHandle));
                            }

                            ImGui::EndDragDropTarget();
                        }
                        break;
                    }
                    case scripting::FieldType::Material: {
                        MonoObject* material = nullptr;
                        fw->getValue(script->getInstance(), &material);

                        if (material) {
                            asset::MaterialAsset* materialHandle = nullptr;
                            scripting::extractMaterial(material, materialHandle);

                            vfs::File* materialFile = editor::State::Project->getVfs()->getFile(materialHandle->getHandle());
                            ImGui::Button(materialFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        } else {
                            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        }

                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__MaterialAsset)) {
                                editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                                vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                                asset::MaterialAsset* newMaterialHandle =
                                    static_cast<asset::MaterialAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                                fw->setValue(script->getInstance(),
                                             ::aderite::Engine::getScriptManager()->getLocator().create(newMaterialHandle));
                            }

                            ImGui::EndDragDropTarget();
                        }
                        break;
                    }
                    case scripting::FieldType::Audio: {
                        MonoObject* audio = nullptr;
                        fw->getValue(script->getInstance(), &audio);

                        if (audio) {
                            asset::AudioAsset* audioHandle = nullptr;
                            scripting::extractAudio(audio, audioHandle);

                            vfs::File* materialFile = editor::State::Project->getVfs()->getFile(audioHandle->getHandle());
                            ImGui::Button(materialFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        } else {
                            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                        }

                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__AudioAsset)) {
                                editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                                vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                                asset::AudioAsset* newAudioHandle =
                                    static_cast<asset::AudioAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                                fw->setValue(script->getInstance(),
                                             ::aderite::Engine::getScriptManager()->getLocator().create(newAudioHandle));
                            }

                            ImGui::EndDragDropTarget();
                        }
                        break;
                    }
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

        if (remove) {
            toRemove.push_back(scripts.Scripts->get(i));
        }
    }

    for (scripting::Script* s : toRemove) {
        scripts.Scripts->removeScript(s);
    }
}

void Inspector::renderAsset() {
    static vfs::File* cacheFile = nullptr;
    static utility::InlineRename renamer;
    io::SerializableObject* object = editor::State::LastSelectedObject.getAsset();

    if (cacheFile == nullptr || cacheFile->getHandle() != object->getHandle()) {
        // Refind
        cacheFile = editor::State::Project->getVfs()->getFile(object->getHandle());
        ADERITE_DYNAMIC_ASSERT(cacheFile != nullptr, "Failed to find associated file");
        renamer.setValue(cacheFile->getName());
    }

    if (renamer.renderUI()) {
        editor::State::Project->getVfs()->rename(cacheFile, renamer.getValue());
        renamer.setValue(renamer.getValue());
    }

    ImGui::Separator();

    switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
    case reflection::RuntimeTypes::MESH: {
        this->renderMesh(object);
        break;
    }
    case reflection::RuntimeTypes::TEXTURE: {
        this->renderTexture(object);
        break;
    }
    case reflection::RuntimeTypes::MATERIAL: {
        this->renderMaterial(object);
        break;
    }
    case reflection::RuntimeTypes::MAT_TYPE: {
        this->renderMaterialType(object);
        break;
    }
    case reflection::RuntimeTypes::SCENE: {
        this->renderScene(object);
        break;
    }
    case reflection::RuntimeTypes::PIPELINE: {
        this->renderPipeline(object);
        break;
    }
    case reflection::RuntimeTypes::CLDR_LIST: {
        this->renderColliderList(object);
        break;
    }
    case reflection::RuntimeTypes::AUDIO: {
        this->renderAudio(object);
        break;
    }
    default: {
    }
    }
}

void Inspector::renderMesh(io::SerializableObject* asset) {
    asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(asset);
    asset::MeshAsset::fields& finfo = mesh->getFieldsMutable();

    // Replace source button
    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
        std::string file = editor_ui::FileDialog::selectFile("Select mesh file");

        if (!file.empty()) {
            ::aderite::Engine::getFileHandler()->writePhysicalFile(mesh->getHandle(), file);
        }
    }
    ImGui::PopItemWidth();

    // Properties

    // Preview
    // TODO: Preview
}

void Inspector::renderTexture(io::SerializableObject* asset) {
    asset::TextureAsset* texture = static_cast<asset::TextureAsset*>(asset);
    asset::TextureAsset::fields& finfo = texture->getFieldsMutable();

    // Replace source button
    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
        std::string file = editor_ui::FileDialog::selectFile("Select image file");

        if (!file.empty()) {
            ::aderite::Engine::getFileHandler()->writePhysicalFile(texture->getHandle(), file);
        }
    }
    ImGui::PopItemWidth();

    // Properties
    // if (ImGui::Checkbox("Is HDR", &finfo.IsHDR)) {
    //	// Unload previous
    //	texture->unload();
    //}

    // Needed for preview
    if (!bgfx::isValid(texture->getTextureHandle())) {
        ImGui::Text("Loading preview...");
        aderite::Engine::getLoaderPool()->enqueue(texture, io::LoaderPool::Priority::HIGH);
        // Can't show yet
        return;
    }

    // Preview
    // TODO: Can be remade to less bloated version
    if (ImGui::BeginTable("TexturePreviewTable", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(1);
        ImGui::Image((void*)(intptr_t)texture->getTextureHandle().idx, ImVec2(96.0f, 96.0f), ImVec2(1, 0), ImVec2(0, 1), ImVec4(1, 1, 1, 1),
                     ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("Preview");

        ImGui::EndTable();
    }
}

void Inspector::renderMaterial(io::SerializableObject* asset) {
    asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(asset);
    asset::MaterialAsset::fields& finfo = material->getFieldsMutable();

    static vfs::File* typeFile = finfo.Type != nullptr ? editor::State::Project->getVfs()->getFile(finfo.Type->getHandle()) : nullptr;

    if (ImGui::BeginTable("MaterialTypeSelectTable", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Material type");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);

        if (typeFile != nullptr) {
            ImGui::Button(typeFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
        } else {
            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__MaterialType)) {
                editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                typeFile = static_cast<vfs::File*>(ddo->Data);
                material->setType(
                    static_cast<aderite::asset::MaterialTypeAsset*>(::aderite::Engine::getSerializer()->getOrRead(typeFile->getHandle())));
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::Text("Properties and samplers:");

    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(material->getFields().Type);

    if (type == nullptr) {
        ImGui::Text("Select material type...");
        return;
    }

    if (ImGui::BeginTable("MaterialEditTable", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

        for (asset::Property* p : type->getProperties()) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(p->getName().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            switch (p->getType()) {
            case asset::PropertyType::FLOAT: {
                float* val = p->getValue(material->getPropertyData());
                ImGui::InputFloat(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
                break;
            }
            case asset::PropertyType::VEC2: {
                float* val = p->getValue(material->getPropertyData());
                ImGui::InputFloat2(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
                break;
            }
            case asset::PropertyType::VEC3: {
                float* val = p->getValue(material->getPropertyData());
                ImGui::InputFloat3(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
                break;
            }
            case asset::PropertyType::VEC4: {
                float* val = p->getValue(material->getPropertyData());
                ImGui::InputFloat4(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
                break;
            }
            default:
                ImGui::Text("Unknown property type");
            }
            ImGui::PopItemWidth();
        }

        auto samplers = type->getSamplers();
        for (size_t i = 0; i < samplers.size(); i++) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(samplers[i]->getName().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            switch (samplers[i]->getType()) {
            case asset::SamplerType::TEXTURE_2D:
            case asset::SamplerType::TEXTURE_CUBE: {
                // TODO: Verify type
                asset::TextureAsset*& sampler = finfo.Samplers[i];
                if (sampler != nullptr) {
                    vfs::File* file = editor::State::Project->getVfs()->getFile(sampler->getHandle());
                    ImGui::Button(file->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                } else {
                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__TextureAsset)) {
                        editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                        vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                        sampler = static_cast<asset::TextureAsset*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle()));
                    }

                    ImGui::EndDragDropTarget();
                }

                break;
            }
            default: {
                ImGui::Text("Unknown sampler type");
            }
            }

            ImGui::PopItemWidth();
        }

        ImGui::EndTable();
    }
}

void Inspector::renderMaterialType(io::SerializableObject* asset) {
    static utility::InlineRename renamer;

    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(asset);
    asset::MaterialTypeAsset::fields& finfo = type->getFieldsMutable();

    ImGui::Text("Properties and samplers:");

    if (ImGui::BeginTable("MaterialTypeEditTable", 4)) {
        ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Spacing", ImGuiTableColumnFlags_WidthFixed, 10.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);

        for (asset::Property* prop : type->getProperties()) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (ImGui::SmallButton("X")) {
                type->removeProperty(prop);
                break;
            }

            ImGui::TableSetColumnIndex(1);
            renamer.setValue(prop->getName());
            if (renamer.renderUI(std::hash<std::string>()(prop->getName()), prop->getName())) {
                prop->setName(renamer.getValue());
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(asset::getNameForType(prop->getType()));
        }

        for (asset::Sampler* samp : type->getSamplers()) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (ImGui::SmallButton("X")) {
                type->removeSampler(samp);
                break;
            }

            ImGui::TableSetColumnIndex(1);
            renamer.setValue(samp->getName());
            if (renamer.renderUI(std::hash<std::string>()(samp->getName()), samp->getName())) {
                samp->setName(renamer.getValue());
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(asset::getNameForType(samp->getType()));
        }
        ImGui::EndTable();
    }

    ImGui::PushItemWidth(-FLT_MIN);

    ImGui::Separator();
    ImGui::Text("Modify:");
    if (ImGui::Button("Add property", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        ImGui::OpenPopup("SelectMaterialTypeProperty");
    }

    if (ImGui::Button("Add sampler", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        ImGui::OpenPopup("SelectMaterialTypeSampler");
    }

    ImGui::Separator();
    ImGui::Text("Compilation:");
    if (ImGui::Button("Open shader editor", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), editor_ui::NodeEditor::NodeEditorType::MATERIAL);
    }

    if (ImGui::Button("Compile", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        type->compile();
    }

    ImGui::PopItemWidth();

    if (ImGui::BeginPopup("SelectMaterialTypeProperty")) {
        ImGui::Text("Type");
        ImGui::Separator();
        for (int i = 0; i < static_cast<int>(asset::PropertyType::COUNT); i++) {
            if (ImGui::Selectable(asset::getNameForType(static_cast<asset::PropertyType>(i)))) {
                type->addProperty(
                    new asset::Property(static_cast<asset::PropertyType>(i), "NewProperty_" + aderite::utility::generateString(8)));
                type->recalculate();
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("SelectMaterialTypeSampler")) {
        ImGui::Text("Type");
        ImGui::Separator();
        for (int i = 0; i < static_cast<int>(asset::SamplerType::COUNT); i++) {
            if (ImGui::Selectable(asset::getNameForType(static_cast<asset::SamplerType>(i)))) {
                type->addSampler(
                    new asset::Sampler(static_cast<asset::SamplerType>(i), "NewSampler_" + aderite::utility::generateString(8)));
                type->recalculate();
            }
        }
        ImGui::EndPopup();
    }
}

void Inspector::renderScene(io::SerializableObject* asset) {
    static vfs::File* cacheFile = nullptr;
    scene::Scene* type = static_cast<scene::Scene*>(asset);

    if (ImGui::BeginTable("SceneEditTable", 2)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Rendering pipeline:");

        ImGui::TableSetColumnIndex(1);
        if (type->getPipeline() == nullptr) {
            ImGui::Selectable("None");
        } else {
            vfs::File* file = editor::State::Project->getVfs()->getFile(type->getPipeline()->getHandle());
            ImGui::Selectable(file->getName().c_str());
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(editor::DDPayloadID__PipelineAsset)) {
                editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
                vfs::File* file = static_cast<vfs::File*>(ddo->Data);
                type->setPipeline(static_cast<rendering::Pipeline*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle())));
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::EndTable();
    }
}

void Inspector::renderPipeline(io::SerializableObject* asset) {
    asset::RenderingPipeline* type = static_cast<asset::RenderingPipeline*>(asset);

    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::Button("Open editor", ImVec2(-1.0f, 0.0f))) {
        WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), editor_ui::NodeEditor::NodeEditorType::RENDER_PIPELINE);
    }

    if (ImGui::Button("Compile", ImVec2(-1.0f, 0.0f))) {
        type->compile();
    }
    ImGui::PopItemWidth();
}

void Inspector::renderColliderList(io::SerializableObject* asset) {
    asset::ColliderListAsset* type = static_cast<asset::ColliderListAsset*>(asset);

    // Colliders
    int i = 0;
    for (physics::Collider* collider : *type) {
        bool remove = false;

        switch (static_cast<reflection::RuntimeTypes>(collider->getType())) {
        case reflection::RuntimeTypes::BOX_CLDR: {
            bool open = false;
            render_component_shared("Box collider " + std::to_string(i++), "Box Collider", open, remove);

            if (open) {
                auto typeCollider = static_cast<physics::BoxCollider*>(collider);
                bool isTrigger = typeCollider->isTrigger();
                glm::vec3 size = typeCollider->getSize();

                if (ImGui::Checkbox("Is trigger", &isTrigger)) {
                    typeCollider->setTrigger(isTrigger);
                    type->incrementIteration();
                }

                if (utility::DrawVec3Control("Size", size, 1.0f)) {
                    typeCollider->setSize(size);
                    type->incrementIteration();
                }

                ImGui::TreePop();
            }
            break;
        }
        default: {
            continue;
        }
        }

        if (remove) {
            type->remove(collider);
            break;
        }
    }

    // Add collider
    ImGui::Separator();

    float width = ImGui::GetContentRegionAvail().x * 0.4855f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
    if (ImGui::Button("Add collider", ImVec2(width, 0.0f))) {
        ImGui::OpenPopup("AddCollider");
    }

    if (ImGui::BeginPopup("AddCollider")) {
        if (ImGui::MenuItem("Box")) {
            type->add(new physics::BoxCollider());
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Inspector::renderAudio(io::SerializableObject* asset) {
    static editor_ui::SelectAudioModal sam;

    asset::AudioAsset* audio = static_cast<asset::AudioAsset*>(asset);

    static vfs::File* audioFile = editor::State::Project->getVfs()->getFile(audio->getHandle());

    if (ImGui::BeginTable("AudioClipEditTable", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Event");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);

        if (ImGui::Button(audio->getEventName().empty() ? "None" : audio->getEventName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
            // Open audio select
            sam.show();
        }

        ImGui::PopItemWidth();
        ImGui::EndTable();
    }

    sam.render();

    if (!sam.getSelectedEvent().empty()) {
        audio->setEventName(sam.getSelectedEvent());

        // Reset the selector
        sam.reset();
    }
}

} // namespace editor_ui
} // namespace aderite
