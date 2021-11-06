#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/scene/Entity.hpp"

#include "aderiteeditor/shared/BaseComponent.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Object inspector used to edit entity and object properties
 */
class Inspector final : public editor::BaseComponent {
public:
    // Inherited via BaseComponent
    virtual void render() override;

private:
    void renderEntity();
    void renderTransform(scene::Entity entity);
    void renderMeshrenderer(scene::Entity entity);
    void renderDynamicBody(scene::Entity entity);
    void renderStaticBody(scene::Entity entity);
    void renderAudioListener(scene::Entity entity);
    void renderColliders(scene::Entity entity);
    void renderScripts(scene::Entity entity);

    void renderAsset();
    void renderMesh(io::SerializableObject* asset);
    void renderTexture(io::SerializableObject* asset);
    void renderMaterial(io::SerializableObject* asset);
    void renderMaterialType(io::SerializableObject* asset);
    void renderScene(io::SerializableObject* asset);
    void renderPipeline(io::SerializableObject* asset);
    void renderColliderList(io::SerializableObject* asset);
    void renderAudio(io::SerializableObject* asset);

    void renderSerializable();
    void renderAudioSource(io::ISerializable* serializable);
};

} // namespace editor_ui
} // namespace aderite