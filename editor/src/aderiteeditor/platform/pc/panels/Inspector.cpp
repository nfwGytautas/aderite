#include "Inspector.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
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
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/geometry/BoxGeometry.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/scripting/events/ScriptGeometryEvent.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/platform/pc/WindowsEditor.hpp"
#include "aderiteeditor/platform/pc/modals/FileDialog.hpp"
#include "aderiteeditor/platform/pc/modals/SelectAudioModal.hpp"
#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"
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
        this->renderMaterialType(object);
        break;
    }
    case reflection::RuntimeTypes::MATERIAL: {
        this->renderMaterial(object);
        break;
    }
    case reflection::RuntimeTypes::GAME_OBJECT: {
        this->renderGameObject(object);
        break;
    }
    default: {
        ImGui::TextWrapped("No properties can be edited for this type");
    }
    }

    ImGui::End();
}

void Inspector::renderTransformProvider(scene::TransformProvider* provider) {
    if (ImGui::CollapsingHeader("Transform")) {
        // Position
        glm::vec3 position = provider->getPosition();
        if (utility::DrawVec3Control("Position", position)) {
            provider->setPosition(position);
        }

        // Rotation
        glm::quat rotationValue = provider->getRotation();
        glm::vec3 euler = glm::eulerAngles(rotationValue);
        glm::vec3 rotation = glm::degrees(euler);
        if (utility::DrawVec3Control("Rotation", rotation)) {
            glm::quat resultValue = glm::quat(glm::radians(rotation));
            provider->setRotation(resultValue);
        }

        // Scale
        glm::vec3 scale = provider->getScale();
        if (utility::DrawVec3Control("Scale", scale, 1.0f)) {
            provider->setScale(scale);
        }
    }
}

void Inspector::renderRenderable(rendering::RenderableData& renderable) {
    if (ImGui::CollapsingHeader("Renderable")) {
        if (ImGui::BeginTable("RenderableTable", 2)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Mesh");

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            if (renderable.getMesh() != nullptr) {
                ImGui::Button(renderable.getMesh()->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
            } else {
                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
            }

            asset::MeshAsset* mesh = DragDrop::renderTarget<asset::MeshAsset>(reflection::RuntimeTypes::MESH);
            if (mesh != nullptr) {
                renderable.setMesh(mesh);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Material");

            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);

            if (renderable.getMaterial() != nullptr) {
                ImGui::Button(renderable.getMaterial()->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
            } else {
                ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
            }

            asset::MaterialAsset* material = DragDrop::renderTarget<asset::MaterialAsset>(reflection::RuntimeTypes::MATERIAL);
            if (material != nullptr) {
                renderable.setMaterial(material);
            }

            ImGui::EndTable();
        }
    }
}

void Inspector::renderActor(physics::PhysXActor* actor) {
    if (ImGui::CollapsingHeader("Rigidbody")) {
        physics::PhysicsProperties& properties = actor->getData();
        bool isStatic = !properties.isDynamic();

        // Static/Dynamic switch
        if (ImGui::Checkbox("Static", &isStatic)) {
            if (isStatic) {
                properties.makeStatic();
            } else {
                properties.makeDynamic();
            }
        }

        if (!isStatic) {
            bool gravity = properties.hasGravity();
            if (ImGui::Checkbox("Has gravity", &gravity)) {
                if (gravity) {
                    properties.enableGravity();
                } else {
                    properties.disableGravity();
                }
            }

            ImGui::Text("Mass");
            ImGui::SameLine();
            float mass = properties.getMass();
            if (ImGui::DragFloat("##X", &mass, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                properties.setMass(mass);
            }
        }
    }
}

void Inspector::renderColliders(physics::PhysXActor* actor) {
    // Colliders
    size_t idx = 1;
    std::vector<physics::Geometry*> geometryToRemove;
    for (physics::Geometry* geom : actor->getData().getAttachedGeometries()) {
        switch (static_cast<reflection::RuntimeTypes>(geom->getType())) {
        case reflection::RuntimeTypes::BOX_GEOMETRY: {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            if (ImGui::Button(("X##boxCollider" + std::to_string(idx)).c_str())) {
                geometryToRemove.push_back(geom);
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();

            if (ImGui::CollapsingHeader(("Box collider##" + std::to_string(idx)).c_str())) {
                physics::BoxGeometry* boxGeom = static_cast<physics::BoxGeometry*>(geom);

                glm::vec3 size = boxGeom->getSize();
                if (utility::DrawVec3Control(std::to_string(idx), "Size", size)) {
                    boxGeom->setSize(size);
                }
            }
            break;
        }
        default: {
            ImGui::Text("Unknown geometry type");
            break;
        }
        }
        idx++;
    }

    for (physics::Geometry* geom : geometryToRemove) {
        actor->getData().removeGeometry(geom);
    }
}

void Inspector::renderCamera(scene::Camera* camera) {
    if (ImGui::CollapsingHeader("Camera")) {
    }
}

void Inspector::renderAudioSource(audio::AudioSource* source) {
    if (ImGui::CollapsingHeader("Audio source")) {
    }
}

void Inspector::renderAudioListener(audio::AudioListener* listener) {
    if (ImGui::CollapsingHeader("Audio listener")) {
    }
}

void Inspector::renderGameObject(io::SerializableObject* object) {
    scene::GameObject* gObject = static_cast<scene::GameObject*>(object);

    // Collect component pointers
    scene::TransformProvider* const transform = gObject->getTransform();
    rendering::Renderable* const renderable = gObject->getRenderable();
    physics::PhysXActor* const actor = gObject->getActor();
    scene::Camera* const camera = gObject->getCamera();
    audio::AudioListener* const listener = gObject->getAudioListener();
    audio::AudioSource* const source = gObject->getAudioSource();

    // Render the game object components
    if (transform != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##transform")) {
            gObject->removeTransform();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderTransformProvider(transform);
    }

    if (renderable != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##renderable")) {
            gObject->removeRenderable();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderRenderable(renderable->getData());
    }

    if (actor != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##actor")) {
            gObject->removeActor();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderActor(actor);

        this->renderColliders(actor);
    }

    if (camera != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##camera")) {
            gObject->removeCamera();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderCamera(camera);
    }

    if (listener != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##audioListener")) {
            gObject->removeAudioListener();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderAudioListener(listener);
    }

    if (source != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("X##audioSource")) {
            gObject->removeAudioSource();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        this->renderAudioSource(source);
    }

    // Add components
    ImGui::Separator();

    float width = ImGui::GetContentRegionAvail().x * 0.4855f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
    if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent")) {
        if (transform == nullptr && ImGui::MenuItem("Transform")) {
            gObject->addTransform();
            ImGui::CloseCurrentPopup();
        }

        if (renderable == nullptr && ImGui::MenuItem("Renderable")) {
            gObject->addRenderable();
            ImGui::CloseCurrentPopup();
        }

        if (actor == nullptr && ImGui::MenuItem("Physics actor")) {
            gObject->addActor();
            ImGui::CloseCurrentPopup();
        }

        if (actor != nullptr) {
            if (ImGui::MenuItem("Box collider")) {
                actor->getData().addGeometry(new physics::BoxGeometry());
                ImGui::CloseCurrentPopup();
            }
        }

        if (camera == nullptr && ImGui::MenuItem("Camera")) {
            gObject->addCamera();
            ImGui::CloseCurrentPopup();
        }

        if (source == nullptr && ImGui::MenuItem("Audio source")) {
            gObject->addAudioSource();
            ImGui::CloseCurrentPopup();
        }

        if (listener == nullptr && ImGui::MenuItem("Audio listener")) {
            gObject->addAudioListener();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Inspector::renderMaterial(io::SerializableObject* asset) {
    asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(asset);

    if (ImGui::BeginTable("MaterialTypeSelectTable", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Material type");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);

        if (material->getMaterialType() != nullptr) {
            ImGui::Button(material->getMaterialType()->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
        } else {
            ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
        }

        asset::MaterialTypeAsset* type = DragDrop::renderTarget<asset::MaterialTypeAsset>(reflection::RuntimeTypes::MAT_TYPE);
        if (type != nullptr) {
            material->setMaterialType(type);
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::Text("Properties and samplers:");

    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(material->getMaterialType());

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
                asset::TextureAsset* sampler = material->getSampler(i);
                if (sampler != nullptr) {
                    ImGui::Button(sampler->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
                } else {
                    ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
                }

                asset::TextureAsset* object = DragDrop::renderTarget<asset::TextureAsset>(reflection::RuntimeTypes::TEXTURE);
                if (object != nullptr) {
                    material->setSampler(i, object);
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

void Inspector::renderMaterialType(io::SerializableObject* object) {
    static utility::InlineRename renamer;

    asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(object);
    asset::MaterialTypeAsset::fields& finfo = type->getFieldsMutable();

    ImGui::Text("Properties and samplers:");

    ImGui::PushItemWidth(-FLT_MIN);

    if (ImGui::Button("Add property", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        ImGui::OpenPopup("SelectMaterialTypeProperty");
    }

    if (ImGui::Button("Add sampler", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        ImGui::OpenPopup("SelectMaterialTypeSampler");
    }

    ImGui::PopItemWidth();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
                type->updateIONodes();
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
                type->updateIONodes();
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(asset::getNameForType(samp->getType()));
        }
        ImGui::EndTable();
    }

    ImGui::PushItemWidth(-FLT_MIN);

    ImGui::Separator();
    ImGui::Text("Compilation:");
    if (ImGui::Button("Compile", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
        type->compile();
    }

    ImGui::PopItemWidth();

    if (ImGui::BeginPopup("SelectMaterialTypeProperty")) {
        ImGui::Text("Type");
        ImGui::Separator();
        for (int i = static_cast<int>(asset::PropertyType::START); i < static_cast<int>(asset::PropertyType::COUNT); i++) {
            if (ImGui::Selectable(asset::getNameForType(static_cast<asset::PropertyType>(i)))) {
                type->addProperty(new asset::Property(static_cast<asset::PropertyType>(i), "p_" + aderite::utility::generateString(8)));
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("SelectMaterialTypeSampler")) {
        ImGui::Text("Type");
        ImGui::Separator();
        for (int i = static_cast<int>(asset::SamplerType::START); i < static_cast<int>(asset::SamplerType::COUNT); i++) {
            if (ImGui::Selectable(asset::getNameForType(static_cast<asset::SamplerType>(i)))) {
                type->addSampler(new asset::Sampler(static_cast<asset::SamplerType>(i), "s_" + aderite::utility::generateString(8)));
            }
        }
        ImGui::EndPopup();
    }
}

} // namespace editor
} // namespace aderite
