#include "AudioListener.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace audio {

AudioListener::AudioListener(scene::GameObject* gObject) : m_gObject(gObject) {}

AudioListener::~AudioListener() {}

void AudioListener::update(float delta) {
    if (!m_data.isEnabled()) {
        return;
    }

    scene::TransformProvider* const transform = m_gObject->getTransform();

    if (transform == nullptr) {
        return;
    }

    FMOD_3D_ATTRIBUTES listener3dAttributes = {};
    const glm::vec3& position = transform->getPosition();

    listener3dAttributes.position = {position.x, position.y, position.z};

    // TODO: Rotation
    listener3dAttributes.up = {0.0f, 1.0f, 0.0f};
    listener3dAttributes.forward = {0.0f, 0.0f, 1.0f};

    // TODO: Velocity
    listener3dAttributes.velocity = {0.0f, 0.0f, 0.0f};

    // TODO: Attenuation

    // Set attributes on FMOD instance
    ::aderite::Engine::getAudioController()->getFmodSystem()->setListenerAttributes(0, &listener3dAttributes, nullptr);
}

AudioListenerData& AudioListener::getData() {
    return m_data;
}

} // namespace audio
} // namespace aderite
