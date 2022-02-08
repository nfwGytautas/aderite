#include "AudioSource.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace audio {

AudioSource::AudioSource(scene::GameObject* gObject) : m_gObject(gObject) {}

AudioSource::~AudioSource() {
    LOG_TRACE("[Audio] Destroying instance {0}", m_gObject->getName());
    this->stop();
}

void AudioSource::update(float delta) {
    if (m_data.getAudioClip() == nullptr) {
        this->stop();
        return;
    }

    if (m_instance == nullptr) {
        // Create audio instance
        m_instance = ::aderite::Engine::getAudioController()->createAudioInstance(m_data.getAudioClip());

        // Set properties
        if (m_data.isMuted()) {
            m_instance->setVolume(0.0f);
        } else {
            m_instance->setVolume(m_data.getVolume());
        }
    }

    scene::TransformProvider* const transform = m_gObject->getTransform();

    if (transform == nullptr) {
        return;
    }

    FMOD_3D_ATTRIBUTES source3dAttributes = {};
    const glm::vec3& position = transform->getPosition();

    source3dAttributes.position = {position.x, position.y, position.z};

    // TODO: Rotation
    source3dAttributes.up = {0.0f, 1.0f, 0.0f};
    source3dAttributes.forward = {0.0f, 0.0f, 1.0f};

    // TODO: Velocity
    source3dAttributes.velocity = {0.0f, 0.0f, 0.0f};

    // Set attributes on FMOD instance
    m_instance->set3DAttributes(&source3dAttributes);
}

void AudioSource::start() const {
    if (m_instance == nullptr) {
        return;
    }

    LOG_TRACE("[Audio] Starting {0}", m_gObject->getName());
    m_instance->start();
}

void AudioSource::stop() const {
    if (m_instance == nullptr) {
        return;
    }

    LOG_TRACE("[Audio] Stopping {0}", m_gObject->getName());
    m_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
}

AudioSourceData& AudioSource::getData() {
    return m_data;
}

} // namespace audio
} // namespace aderite
