#include "AudioInstance.hpp"

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace audio {

AudioInstance::AudioInstance(FMOD::Studio::EventInstance* instance) : m_instance(instance) {}

AudioInstance::~AudioInstance() {
    LOG_TRACE("[Audio] Destroying instance {0}", m_name);
    this->stop();
}

void AudioInstance::start() const {
    LOG_TRACE("[Audio] Starting {0}", m_name);
    m_instance->start();
}

void AudioInstance::stop() const {
    LOG_TRACE("[Audio] Stopping {0}", m_name);
    m_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
}

void AudioInstance::setVolume(float volume) const {
    m_instance->setVolume(volume);
}

void AudioInstance::setAttributes(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& velocity) const {
    FMOD_3D_ATTRIBUTES source3dAttributes = {};
    source3dAttributes.position = {position.x, position.y, position.z};
    source3dAttributes.velocity = {velocity.x, velocity.y, velocity.z};

    // TODO: Rotation
    source3dAttributes.up = {0.0f, 1.0f, 0.0f};
    source3dAttributes.forward = {0.0f, 0.0f, 1.0f};

    // Set attributes on FMOD instance
    m_instance->set3DAttributes(&source3dAttributes);
}

} // namespace audio
} // namespace aderite
