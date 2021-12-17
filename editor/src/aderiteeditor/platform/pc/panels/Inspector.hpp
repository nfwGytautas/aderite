#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Object inspector used to edit entity and object properties
 */
class Inspector final : public UIComponent {
public:
    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    // Common
    void renderTransformProvider(scene::ITransformProvider* provider);
    void renderRenderable(rendering::Renderable* renderable);

    // Objects
    void renderVisual(io::SerializableObject* object);
    void renderScenery(io::SerializableObject* object);
    void renderEntity(io::SerializableObject* object);

    // Physics
    void renderBaseGeometry(physics::Geometry* geometry);
    void renderBaseActor(physics::BasePhysicsActor* actor);
    void renderStaticActor(physics::StaticPhysicsActor* actor);
    void renderDynamicActor(physics::DynamicPhysicsActor* actor);

    // Assets
    // void renderAsset();
    // void renderMesh(io::SerializableAsset* asset);
    // void renderTexture(io::SerializableAsset* asset);
    void renderMaterial(io::SerializableObject* asset);
    void renderMaterialType(io::SerializableObject* object);
    // void renderScene(io::SerializableAsset* asset);
    // void renderPipeline(io::SerializableAsset* asset);
    // void renderAudio(io::SerializableAsset* asset);

    //// Serializables
    // void renderSerializable();
    // void renderAudioSource(io::SerializableObject* serializable);
    // void renderScriptSystem(io::SerializableObject* serializable);
    // void renderAudioListener(io::SerializableObject* serializable);
};

} // namespace editor
} // namespace aderite