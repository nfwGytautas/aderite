#pragma once

#include "aderite/audio/Forward.hpp"
#include "aderite/io/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

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
    void renderBehavior(scripting::ScriptedBehavior* behavior, size_t idx);

    // Objects
    void renderGameObject(io::SerializableObject* object);

    // Physics
    void renderActor(physics::PhysXActor* actor);
    void renderColliders(physics::PhysXActor* actor);

    // Assets
    void renderMaterial(io::SerializableObject* asset);
    void renderMaterialType(io::SerializableObject* object);
    void renderAudioClip(io::SerializableObject* asset);
};

} // namespace editor
} // namespace aderite