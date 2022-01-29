#pragma once

#include "aderite/audio/Forward.hpp"
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
    // Components
    void renderTransformProvider(scene::TransformProvider* provider);
    void renderRenderable(rendering::RenderableData& renderable);
    void renderCamera(scene::Camera* camera);
    void renderAudioSource(audio::AudioSource* source);
    void renderAudioListener(audio::AudioListener* listener);

    // Objects
    void renderGameObject(io::SerializableObject* object);

    // Physics
    void renderActor(physics::PhysXActor* actor);
    void renderBaseGeometry(physics::Geometry* geometry);

    // Assets
    void renderMaterial(io::SerializableObject* asset);
    void renderMaterialType(io::SerializableObject* object);
};

} // namespace editor
} // namespace aderite