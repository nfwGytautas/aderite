#pragma once

#include "aderite/io/Forward.hpp"
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
    //// Entity
    //void renderEntity();

    //// Physics
    //void renderActor(physics::PhysicsActor* actor);
    //void renderDynamicActor(physics::DynamicActor* actor);
    //void renderStaticActor(physics::StaticActor* actor);

    // Assets
    //void renderAsset();
    //void renderMesh(io::SerializableAsset* asset);
    //void renderTexture(io::SerializableAsset* asset);
    //void renderMaterial(io::SerializableAsset* asset);
    void renderMaterialType(io::SerializableObject* object);
    //void renderScene(io::SerializableAsset* asset);
    //void renderPipeline(io::SerializableAsset* asset);
    //void renderAudio(io::SerializableAsset* asset);

    //// Serializables
    //void renderSerializable();
    //void renderAudioSource(io::SerializableObject* serializable);
    //void renderScriptSystem(io::SerializableObject* serializable);
    //void renderAudioListener(io::SerializableObject* serializable);
};

} // namespace editor
} // namespace aderite