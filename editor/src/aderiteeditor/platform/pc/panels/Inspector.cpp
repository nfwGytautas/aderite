#include "Inspector.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
//#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/platform/pc/modals/FileDialog.hpp"
#include "aderiteeditor/platform/pc/modals/SelectAudioModal.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"
#include "aderiteeditor/utility/Utility.hpp"

namespace aderite {
namespace editor {

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

// void Inspector::renderEntity() {
//    static utility::InlineRename renamer;
//    scene::Entity* entity = editor::State::LastSelectedObject.getEntity();
//
//    renamer.setValue(entity->getName());
//
//    if (renamer.renderUI()) {
//        entity->setName(renamer.getValue());
//    }
//
//    ImGui::Separator();
//
//    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
//    if (ImGui::BeginTabBar("EntityTabBar", ImGuiTabBarFlags_None)) {
//        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
//
//        if (ImGui::BeginTabItem("Base")) {
//            ImGui::Text("Transform");
//
//            /*scene::Transform* transform = entity->getTransform();
//
//            if (utility::DrawVec3Control("Position", transform->position())) {
//                if (entity->getActor() != nullptr) {
//                    entity->getActor()->moveActor(transform->position());
//                }
//            }
//
//            glm::vec3 euler = glm::eulerAngles(transform->rotation());
//            glm::vec3 rotation = glm::degrees(euler);
//            if (utility::DrawVec3Control("Rotation", rotation)) {
//                transform->rotation() = glm::quat(rotation);
//
//                if (entity->getActor() != nullptr) {
//                    entity->getActor()->rotateActor(transform->rotation());
//                }
//            }
//
//            if (utility::DrawVec3Control("Scale", transform->scale(), 1.0f)) {
//                if (entity->getActor() != nullptr) {
//                    for (physics::Collider* collider : entity->getActor()->getColliders()) {
//                        collider->setScale(transform->scale());
//                    }
//                }
//            }
//
//            ImGui::Separator();
//            ImGui::Text("Tags");
//            if (ImGui::BeginListBox("##taglist", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
//                auto& tags = entity->getScene()->getTags();
//                for (size_t i = 0; i < tags.size(); i++) {
//                    if (tags[i].empty()) {
//                        continue;
//                    }
//
//                    size_t tag = 1Ui64 << i;
//                    bool has = entity->hasTag(tag);
//
//                    if (ImGui::Checkbox(tags[i].c_str(), &has)) {
//                        if (has) {
//                            entity->addTag(tag);
//                        } else {
//                            entity->removeTag(tag);
//                        }
//                    }
//                }
//
//                ImGui::EndListBox();
//            }*/
//
//            ImGui::EndTabItem();
//        }
//
//        if (ImGui::BeginTabItem("Rendering")) {
//            rendering::Renderable* renderable = entity->getRenderable();
//
//            // Type radio button
//            int type = renderable == nullptr ? 0 : static_cast<int>(renderable->getType());
//
//            if (ImGui::RadioButton("No rendering", &type, 0)) {
//                if (renderable != nullptr) {
//                    // TODO: Delete renderable
//                }
//            }
//
//            ImGui::SameLine();
//            if (ImGui::RadioButton("Render", &type, static_cast<int>(reflection::RuntimeTypes::RENDERABLE))) {
//                if (renderable != nullptr) {
//                    // TODO: Delete and create renderable
//                }
//
//                rendering::Renderable* renderable = new rendering::Renderable();
//                entity->setRenderable(renderable);
//            }
//
//            ImGui::Separator();
//
//            // Renderable
//            if (renderable != nullptr) {
//                if (ImGui::BeginTable("RenderableTable", 2)) {
//                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
//                    ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//                    ImGui::TableNextRow();
//
//                    ImGui::TableSetColumnIndex(0);
//                    ImGui::Text("Mesh");
//
//                    ImGui::TableSetColumnIndex(1);
//                    ImGui::PushItemWidth(-FLT_MIN);
//
//                    if (renderable->getMesh()) {
//                        vfs::File* meshFile = editor::State::Project->getVfs()->getFile(renderable->getMesh()->getHandle());
//                        ImGui::Button(meshFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                    } else {
//                        ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                    }
//
//                    asset::MeshAsset* mesh = DragDrop::renderTarget<asset::MeshAsset>(reflection::RuntimeTypes::MESH);
//                    if (mesh != nullptr) {
//                        renderable->setMesh(mesh);
//                    }
//
//                    ImGui::TableNextRow();
//                    ImGui::TableSetColumnIndex(0);
//                    ImGui::Text("Material");
//
//                    ImGui::TableSetColumnIndex(1);
//                    ImGui::PushItemWidth(-FLT_MIN);
//
//                    if (renderable->getMaterial()) {
//                        vfs::File* matFile = editor::State::Project->getVfs()->getFile(renderable->getMaterial()->getHandle());
//                        ImGui::Button(matFile->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                    } else {
//                        ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                    }
//
//                    asset::MaterialAsset* material = DragDrop::renderTarget<asset::MaterialAsset>(reflection::RuntimeTypes::MATERIAL);
//                    if (material != nullptr) {
//                        renderable->setMaterial(material);
//                    }
//
//                    ImGui::EndTable();
//                }
//            }
//
//            ImGui::EndTabItem();
//        }
//
//        //if (ImGui::BeginTabItem("Physics")) {
//        //    physics::PhysicsActor* actor = entity->getActor();
//
//        //    // Type radio button
//        //    int type = actor == nullptr ? 0 : static_cast<int>(actor->getType());
//
//        //    if (ImGui::RadioButton("No actor", &type, 0)) {
//        //        if (actor != nullptr) {
//        //            entity->setActor(nullptr);
//        //        }
//        //    }
//
//        //    ImGui::SameLine();
//        //    if (ImGui::RadioButton("Static", &type, static_cast<int>(reflection::RuntimeTypes::STATIC_ACTOR))) {
//        //        if (actor != nullptr) {
//        //            // Delete old actor if different type
//        //            if (actor->getType() != static_cast<int>(reflection::RuntimeTypes::STATIC_ACTOR)) {
//        //                entity->setActor(new physics::StaticActor(), true);
//        //            }
//        //        } else {
//        //            entity->setActor(new physics::StaticActor());
//        //        }
//        //    }
//
//        //    ImGui::SameLine();
//        //    if (ImGui::RadioButton("Dynamic", &type, static_cast<int>(reflection::RuntimeTypes::DYNAMIC_ACTOR))) {
//        //        if (actor != nullptr) {
//        //            // Delete old actor if different type
//        //            if (actor->getType() != static_cast<int>(reflection::RuntimeTypes::DYNAMIC_ACTOR)) {
//        //                entity->setActor(new physics::DynamicActor(), true);
//        //            }
//        //        } else {
//        //            entity->setActor(new physics::DynamicActor());
//        //        }
//        //    }
//
//        //    ImGui::Separator();
//
//        //    // Refresh actor in case its type was changed
//        //    actor = entity->getActor();
//
//        //    // Actor
//        //    if (actor != nullptr) {
//        //        // Properties
//        //        switch (static_cast<reflection::RuntimeTypes>(actor->getType())) {
//        //        case reflection::RuntimeTypes::DYNAMIC_ACTOR: {
//        //            this->renderDynamicActor(static_cast<physics::DynamicActor*>(actor));
//        //            break;
//        //        }
//        //        case reflection::RuntimeTypes::STATIC_ACTOR: {
//        //            this->renderStaticActor(static_cast<physics::StaticActor*>(actor));
//        //            break;
//        //        }
//        //        }
//        //    }
//
//        //    ImGui::EndTabItem();
//        //}
//
//        ImGui::PopStyleVar();
//
//        ImGui::EndTabBar();
//    }
//}

// void Inspector::renderActor(physics::PhysicsActor* actor) {
//    // Colliders
//    ImGui::Separator();
//    ImGui::Text("Colliders");
//
//    // Add collider
//    ImGui::Separator();
//
//    float width = ImGui::GetContentRegionAvail().x * 0.4855f;
//    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
//    if (ImGui::Button("Add collider", ImVec2(width, 0.0f))) {
//        ImGui::OpenPopup("AddCollider");
//    }
//
//    if (ImGui::BeginPopup("AddCollider")) {
//        if (ImGui::MenuItem("Box")) {
//            actor->addCollider(new physics::BoxCollider());
//            ImGui::CloseCurrentPopup();
//        }
//
//        ImGui::EndPopup();
//    }
//
//    // Collider list
//    size_t colliderIdx = 0;
//    for (physics::Collider* collider : actor->getColliders()) {
//        ImGui::Text("Collider %ld", colliderIdx);
//        colliderIdx++;
//    }
//}
//
// void Inspector::renderDynamicActor(physics::DynamicActor* actor) {
//    // Properties
//    bool hasGravity = actor->getGravity();
//    if (ImGui::Checkbox("Has gravity", &hasGravity)) {
//        actor->setGravity(hasGravity);
//    }
//
//    bool isKinematic = actor->getKinematic();
//    if (ImGui::Checkbox("Is kinematic", &isKinematic)) {
//        actor->setKinematic(isKinematic);
//    }
//
//    ImGui::Text("Mass");
//    ImGui::SameLine();
//    float mass = actor->getMass();
//    if (ImGui::DragFloat("##X", &mass, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
//        actor->setMass(mass);
//    }
//
//    // Shared
//    this->renderActor(actor);
//}
//
// void Inspector::renderStaticActor(physics::StaticActor* actor) {
//    // Properties
//    ImGui::Text("Static actors don't have properties yet");
//
//    // Shared
//    this->renderActor(actor);
//}

//void Inspector::renderAsset() {
//    // static vfs::File* cacheFile = nullptr;
//    // static utility::InlineRename renamer;
//    // io::SerializableAsset* object = editor::State::LastSelectedObject.getAsset();
//
//    // if (cacheFile == nullptr || cacheFile->getHandle() != object->getHandle()) {
//    //    // Refind
//    //    cacheFile = editor::State::Project->getVfs()->getFile(object->getHandle());
//    //    ADERITE_DYNAMIC_ASSERT(cacheFile != nullptr, "Failed to find associated file");
//    //    renamer.setValue(cacheFile->getName());
//    //}
//
//    // if (renamer.renderUI()) {
//    //    editor::State::Project->getVfs()->rename(cacheFile, renamer.getValue());
//    //    renamer.setValue(renamer.getValue());
//    //}
//
//    // ImGui::Separator();
//
//    // switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
//    // case reflection::RuntimeTypes::MESH: {
//    //    this->renderMesh(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::TEXTURE: {
//    //    this->renderTexture(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::MATERIAL: {
//    //    this->renderMaterial(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::MAT_TYPE: {
//    //    this->renderMaterialType(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::SCENE: {
//    //    this->renderScene(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::PIPELINE: {
//    //    this->renderPipeline(object);
//    //    break;
//    //}
//    // case reflection::RuntimeTypes::AUDIO: {
//    //    this->renderAudio(object);
//    //    break;
//    //}
//    // default: {
//    //}
//    //}
//}
//
//void Inspector::renderMesh(io::SerializableAsset* asset) {
//    // asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(asset);
//    // asset::MeshAsset::fields& finfo = mesh->getFieldsMutable();
//
//    //// Replace source button
//    // ImGui::PushItemWidth(-FLT_MIN);
//    // if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
//    //    std::string file = FileDialog::selectFile("Select mesh file");
//
//    //    if (!file.empty()) {
//    //        ::aderite::Engine::getFileHandler()->writePhysicalFile(mesh->getHandle(), file);
//    //    }
//    //}
//    // ImGui::PopItemWidth();
//
//    // Properties
//
//    // Preview
//    // TODO: Preview
//}
//
//void Inspector::renderTexture(io::SerializableAsset* asset) {
//    asset::TextureAsset* texture = static_cast<asset::TextureAsset*>(asset);
//    asset::TextureAsset::fields& finfo = texture->getFieldsMutable();
//
//    // Replace source button
//    ImGui::PushItemWidth(-FLT_MIN);
//    if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
//        std::string file = FileDialog::selectFile("Select image file");
//
//        if (!file.empty()) {
//            ::aderite::Engine::getFileHandler()->writePhysicalFile(texture->getHandle(), file);
//        }
//    }
//    ImGui::PopItemWidth();
//
//    // Properties
//    // if (ImGui::Checkbox("Is HDR", &finfo.IsHDR)) {
//    //	// Unload previous
//    //	texture->unload();
//    //}
//
//    // Needed for preview
//    if (!bgfx::isValid(texture->getTextureHandle())) {
//        ImGui::Text("Loading preview...");
//        aderite::Engine::getLoaderPool()->enqueue(texture, io::LoaderPool::Priority::HIGH);
//        // Can't show yet
//        return;
//    }
//
//    // Preview
//    // TODO: Can be remade to less bloated version
//    if (ImGui::BeginTable("TexturePreviewTable", 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        ImGui::TableNextRow();
//
//        ImGui::TableSetColumnIndex(1);
//        ImGui::Image((void*)(intptr_t)texture->getTextureHandle().idx, ImVec2(96.0f, 96.0f), ImVec2(1, 0), ImVec2(0, 1), ImVec4(1, 1, 1, 1),
//                     ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
//        ImGui::Text("Preview");
//
//        ImGui::EndTable();
//    }
//}
//
//void Inspector::renderMaterial(io::SerializableAsset* asset) {
//    asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(asset);
//    asset::MaterialAsset::fields& finfo = material->getFieldsMutable();
//
//    if (ImGui::BeginTable("MaterialTypeSelectTable", 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        ImGui::TableNextRow();
//
//        ImGui::TableSetColumnIndex(0);
//        ImGui::Text("Material type");
//
//        ImGui::TableSetColumnIndex(1);
//        ImGui::PushItemWidth(-FLT_MIN);
//
//        if (finfo.Type != nullptr) {
//            ImGui::Button(finfo.Type->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//        } else {
//            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//        }
//
//        asset::MaterialTypeAsset* type = DragDrop::renderTarget<asset::MaterialTypeAsset>(reflection::RuntimeTypes::MAT_TYPE);
//        if (type != nullptr) {
//            material->setType(type);
//        }
//
//        ImGui::EndTable();
//    }
//
//    ImGui::Separator();
//    ImGui::Text("Properties and samplers:");
//
//    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(material->getFields().Type);
//
//    if (type == nullptr) {
//        ImGui::Text("Select material type...");
//        return;
//    }
//
//    if (ImGui::BeginTable("MaterialEditTable", 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        for (asset::Property* p : type->getProperties()) {
//            ImGui::TableNextRow();
//
//            ImGui::TableSetColumnIndex(0);
//            ImGui::Text(p->getName().c_str());
//
//            ImGui::TableSetColumnIndex(1);
//            ImGui::PushItemWidth(-FLT_MIN);
//
//            switch (p->getType()) {
//            case asset::PropertyType::FLOAT: {
//                float* val = p->getValue(material->getPropertyData());
//                ImGui::InputFloat(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
//                break;
//            }
//            case asset::PropertyType::VEC2: {
//                float* val = p->getValue(material->getPropertyData());
//                ImGui::InputFloat2(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
//                break;
//            }
//            case asset::PropertyType::VEC3: {
//                float* val = p->getValue(material->getPropertyData());
//                ImGui::InputFloat3(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
//                break;
//            }
//            case asset::PropertyType::VEC4: {
//                float* val = p->getValue(material->getPropertyData());
//                ImGui::InputFloat4(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
//                break;
//            }
//            default:
//                ImGui::Text("Unknown property type");
//            }
//            ImGui::PopItemWidth();
//        }
//
//        auto samplers = type->getSamplers();
//        for (size_t i = 0; i < samplers.size(); i++) {
//            ImGui::TableNextRow();
//
//            ImGui::TableSetColumnIndex(0);
//            ImGui::Text(samplers[i]->getName().c_str());
//
//            ImGui::TableSetColumnIndex(1);
//            ImGui::PushItemWidth(-FLT_MIN);
//
//            switch (samplers[i]->getType()) {
//            case asset::SamplerType::TEXTURE_2D:
//            case asset::SamplerType::TEXTURE_CUBE: {
//                // TODO: Verify type
//                asset::TextureAsset*& sampler = finfo.Samplers[i];
//                if (sampler != nullptr) {
//                    ImGui::Button(sampler->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                asset::TextureAsset* object = DragDrop::renderTarget<asset::TextureAsset>(reflection::RuntimeTypes::TEXTURE);
//                if (object != nullptr) {
//                    sampler = object;
//                }
//
//                break;
//            }
//            default: {
//                ImGui::Text("Unknown sampler type");
//            }
//            }
//
//            ImGui::PopItemWidth();
//        }
//
//        ImGui::EndTable();
//    }
//}

void Inspector::renderMaterialType(io::SerializableObject* object) {
    static utility::InlineRename renamer;

    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(object);
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
        // WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), editor_ui::NodeEditor::NodeEditorType::MATERIAL);
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

//void Inspector::renderScene(io::SerializableAsset* asset) {
//    scene::Scene* type = static_cast<scene::Scene*>(asset);
//
//    if (ImGui::BeginTable("SceneEditTable", 2)) {
//        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
//
//        ImGui::TableNextRow();
//        ImGui::TableSetColumnIndex(0);
//        ImGui::Text("Rendering pipeline:");
//
//        ImGui::TableSetColumnIndex(1);
//        if (type->getPipeline() == nullptr) {
//            ImGui::Selectable("None");
//        } else {
//            ImGui::Selectable(type->getPipeline()->getName().c_str());
//        }
//
//        rendering::Pipeline* object = DragDrop::renderTarget<rendering::Pipeline>(reflection::RuntimeTypes::PIPELINE);
//        if (object != nullptr) {
//            type->setPipeline(object);
//        }
//
//        ImGui::EndTable();
//    }
//}
//
//void Inspector::renderPipeline(io::SerializableAsset* asset) {
//    asset::RenderingPipeline* type = static_cast<asset::RenderingPipeline*>(asset);
//
//    ImGui::PushItemWidth(-FLT_MIN);
//    if (ImGui::Button("Open editor", ImVec2(-1.0f, 0.0f))) {
//        // WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), editor_ui::NodeEditor::NodeEditorType::RENDER_PIPELINE);
//    }
//
//    if (ImGui::Button("Compile", ImVec2(-1.0f, 0.0f))) {
//        type->compile();
//    }
//    ImGui::PopItemWidth();
//}
//
//void Inspector::renderAudio(io::SerializableAsset* asset) {
//    static SelectAudioModal sam;
//    asset::AudioAsset* audio = static_cast<asset::AudioAsset*>(asset);
//
//    if (ImGui::BeginTable("AudioClipEditTable", 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        ImGui::TableNextRow();
//
//        ImGui::TableSetColumnIndex(0);
//        ImGui::Text("Event");
//
//        ImGui::TableSetColumnIndex(1);
//        ImGui::PushItemWidth(-FLT_MIN);
//
//        if (ImGui::Button(audio->getEventName().empty() ? "None" : audio->getEventName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
//            // Open audio select
//            sam.show();
//        }
//
//        ImGui::PopItemWidth();
//        ImGui::EndTable();
//    }
//
//    sam.render();
//
//    if (!sam.getSelectedEvent().empty()) {
//        audio->setEventName(sam.getSelectedEvent());
//
//        // Reset the selector
//        sam.reset();
//    }
//}
//
//void Inspector::renderSerializable() {
//    io::SerializableObject* object = editor::State::LastSelectedObject.getSerializable();
//
//    switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
//    case reflection::RuntimeTypes::AUDIO_SOURCE: {
//        this->renderAudioSource(object);
//        break;
//    }
//    case reflection::RuntimeTypes::SCRIPT_SYSTEM: {
//        this->renderScriptSystem(object);
//        break;
//    }
//    /*case reflection::RuntimeTypes::TAG_SELECTOR: {
//        this->renderEntitySelector(object);
//        break;
//    }*/
//    case reflection::RuntimeTypes::AUDIO_LISTENER: {
//        this->renderAudioListener(object);
//        break;
//    }
//    default: {
//    }
//    }
//}
//
//void Inspector::renderAudioSource(io::SerializableObject* serializable) {
//    static utility::InlineRename renamer;
//    audio::AudioSource* source = static_cast<audio::AudioSource*>(serializable);
//
//    renamer.setValue(source->getName());
//
//    if (renamer.renderUI()) {
//        source->setName(renamer.getValue());
//        renamer.setValue(renamer.getValue());
//    }
//
//    ImGui::Separator();
//}
//
//void Inspector::renderScriptSystem(io::SerializableObject* serializable) {
//    scripting::ScriptSystem* system = static_cast<scripting::ScriptSystem*>(serializable);
//
//    ImGui::Text(system->getName().c_str());
//
//    if (ImGui::BeginTable("ScriptSystemEditTable", 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        ImGui::TableNextRow();
//
//        ImGui::TableSetColumnIndex(0);
//        ImGui::Text("Selector");
//
//        ImGui::TableSetColumnIndex(1);
//        ImGui::PushItemWidth(-FLT_MIN);
//
//        /*if (system->getSelector() != nullptr) {
//            ImGui::Button(system->getSelector()->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//        } else {
//            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//        }
//
//        scene::EntitySelector* object = DragDrop::renderTarget<scene::EntitySelector>(reflection::RuntimeTypes::ENTITY_SELECTOR);
//        if (object != nullptr) {
//            system->setSelector(static_cast<scene::EntitySelector*>(object));
//        }*/
//
//        ImGui::EndTable();
//    }
//
//    // Fields
//    if (ImGui::BeginTable((system->getName() + "EditTable").c_str(), 2)) {
//        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
//
//        // Render fields
//        for (const scripting::FieldWrapper& fw : system->getFields()) {
//            ImGui::TableNextRow();
//
//            ImGui::TableSetColumnIndex(0);
//            ImGui::Text(fw.getName().c_str());
//
//            ImGui::TableSetColumnIndex(1);
//            ImGui::PushItemWidth(-FLT_MIN);
//
//            switch (fw.getType()) {
//            case scripting::FieldType::Float: {
//                float val = fw.getValueType<float>();
//                if (ImGui::InputFloat(std::string("#" + fw.getName()).c_str(), &val, NULL)) {
//                    fw.setValueType(val);
//                }
//                break;
//            }
//            case scripting::FieldType::Boolean: {
//                bool val = fw.getValueType<bool>();
//                if (ImGui::Checkbox(std::string("##" + fw.getName()).c_str(), &val)) {
//                    fw.setValueType(val);
//                }
//                break;
//            }
//            case scripting::FieldType::Mesh: {
//                asset::MeshAsset* handle = static_cast<asset::MeshAsset*>(fw.getSerializable());
//
//                if (handle) {
//                    ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                asset::MeshAsset* object = DragDrop::renderTarget<asset::MeshAsset>(reflection::RuntimeTypes::MESH);
//                if (object != nullptr) {
//                    fw.setSerializable(object);
//                }
//
//                break;
//            }
//            case scripting::FieldType::Material: {
//                asset::MaterialAsset* handle = static_cast<asset::MaterialAsset*>(fw.getSerializable());
//
//                if (handle) {
//                    ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                asset::MaterialAsset* object = DragDrop::renderTarget<asset::MaterialAsset>(reflection::RuntimeTypes::MATERIAL);
//                if (object != nullptr) {
//                    fw.setSerializable(object);
//                }
//
//                break;
//            }
//            case scripting::FieldType::Audio: {
//                asset::AudioAsset* handle = static_cast<asset::AudioAsset*>(fw.getSerializable());
//
//                if (handle) {
//                    ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                asset::AudioAsset* object = DragDrop::renderTarget<asset::AudioAsset>(reflection::RuntimeTypes::AUDIO);
//                if (object != nullptr) {
//                    fw.setSerializable(object);
//                }
//
//                break;
//            }
//            /*case scripting::FieldType::AudioSource: {
//                audio::AudioSource* handle = static_cast<audio::AudioSource*>(fw.getSerializable());
//
//                if (handle) {
//                    ImGui::Button(handle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                audio::AudioSource* object = DragDrop::renderTarget<audio::AudioSource>(reflection::RuntimeTypes::AUDIO_SOURCE);
//                if (object != nullptr) {
//                    fw.setSerializable(object);
//                }
//
//                break;
//            }*/
//            /*case scripting::FieldType::Prefab: {
//                asset::PrefabAsset* handle = static_cast<asset::PrefabAsset*>(fw.getSerializable());
//
//                if (handle) {
//                    vfs::File* file = editor::State::Project->getVfs()->getFile(handle->getHandle());
//                    ImGui::Button(file->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                } else {
//                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
//                }
//
//                asset::PrefabAsset* object = DragDrop::renderTarget<asset::PrefabAsset>(reflection::RuntimeTypes::PREFAB);
//                if (object != nullptr) {
//                    fw.setSerializable(object);
//                }
//
//                break;
//            }*/
//            default: {
//                ImGui::Text("Unknown field type");
//            }
//            }
//
//            ImGui::PopItemWidth();
//        }
//
//        ImGui::EndTable();
//    }
//}
//
//void Inspector::renderAudioListener(io::SerializableObject* serializable) {
//    static utility::InlineRename renamer;
//    audio::AudioListener* listener = static_cast<audio::AudioListener*>(serializable);
//
//    renamer.setValue(listener->getName());
//
//    if (renamer.renderUI()) {
//        listener->setName(renamer.getValue());
//        renamer.setValue(renamer.getValue());
//    }
//
//    ImGui::Separator();
//
//    bool enabled = listener->isEnabled();
//    if (ImGui::Checkbox("Enabled", &enabled)) {
//        if (enabled) {
//            listener->enable();
//        } else {
//            listener->disable();
//        }
//    }
//}

bool Inspector::init() {
    return true;
}

void Inspector::shutdown() {}

void Inspector::render() {
    static utility::InlineRename renamer;
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    if (!ImGui::Begin("Inspector")) {
        ImGui::End();
        return;
    }

    // Shared UI elements
    io::SerializableObject* object = State::getInstance().getSelectedObject();

    if (object == nullptr) {
        ImGui::TextWrapped("Select object either from scene hierarchy or asset browser");
        ImGui::End();
        return;
    }

    // Name
    if (renamer.renderUI(std::hash<std::string>()(object->getName()), object->getName())) {
        object->setName(renamer.getValue());
    }

    ImGui::Separator();

    switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
    case reflection::RuntimeTypes::MAT_TYPE: {

        break;
    }
    default: {
        ImGui::TextWrapped("No properties can be edited for this type");
    }
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
