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
    // Entity
    void renderEntity();

    // Physics
    void renderActor(physics::PhysicsActor* actor);
    void renderDynamicActor(physics::DynamicActor* actor);
    void renderStaticActor(physics::StaticActor* actor);

    // Assets
    void renderAsset();
    void renderMesh(io::SerializableObject* asset);
    void renderTexture(io::SerializableObject* asset);
    void renderMaterial(io::SerializableObject* asset);
    void renderMaterialType(io::SerializableObject* asset);
    void renderScene(io::SerializableObject* asset);
    void renderPipeline(io::SerializableObject* asset);
    void renderAudio(io::SerializableObject* asset);

    // Serializables
    void renderSerializable();
    void renderAudioSource(io::ISerializable* serializable);
    void renderScriptSystem(io::ISerializable* serializable);
    void renderEntitySelector(io::ISerializable* serializable);
};

} // namespace editor_ui
} // namespace aderite