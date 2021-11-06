#include "AudioSource.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace audio {

static audio::SourceHandle g_NextSourceHandle = 0;

AudioSource::AudioSource() : m_handle(g_NextSourceHandle++) {}

AudioSource::~AudioSource() {}

void AudioSource::update() {
    // TODO: Clean one shots

    for (AudioInstance* instance : m_instances) {
        this->syncInstance(instance);
    }

    for (AudioInstance* oneshot : m_oneshots) {
        this->syncInstance(oneshot);
    }
}

AudioInstance* AudioSource::createInstance(const asset::AudioAsset* clip) {
    AudioInstance* instance = ::aderite::Engine::getAudioController()->createAudioInstance(clip->getEventName());
    m_instances.push_back(instance);
    return instance;
}

AudioInstance* AudioSource::createOneshot(const asset::AudioAsset* clip) {
    AudioInstance* instance = ::aderite::Engine::getAudioController()->createAudioInstance(clip->getEventName());
    instance->start();
    m_oneshots.push_back(instance);
    return instance;
}

void AudioSource::mute() {
    m_muted = true;
}

void AudioSource::unmute() {
    m_muted = false;
}

void AudioSource::stop() {
    for (AudioInstance* instance : m_instances) {
        instance->stop();
    }

    for (AudioInstance* oneshot : m_oneshots) {
        oneshot->stop();
    }
}

void AudioSource::setVolume(const float volume) {
    m_volume = volume;
}

void AudioSource::setPosition(const glm::vec3& position) {
    m_position = position;
}

void AudioSource::setRotation(const glm::quat& rotation) {
    m_rotation = rotation;
}

void AudioSource::setVelocity(const glm::vec3& velocity) {
    m_velocity = velocity;
}

reflection::Type AudioSource::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::AUDIO_SOURCE);
}

bool AudioSource::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Handle" << YAML::Value << m_handle;
    emitter << YAML::Key << "Volume" << YAML::Value << m_volume;
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_rotation;
    emitter << YAML::Key << "Velocity" << YAML::Value << m_velocity;
    return true;
}

bool AudioSource::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_handle = data["Handle"].as<audio::SourceHandle>();
    m_volume = data["Volume"].as<float>();
    m_position = data["Position"].as<glm::vec3>();
    m_rotation = data["Rotation"].as<glm::quat>();
    m_velocity = data["Velocity"].as<glm::vec3>();

    if (g_NextSourceHandle < m_handle) {
        g_NextSourceHandle = m_handle + 1;
    }

    return true;
}

void AudioSource::syncInstance(AudioInstance* instance) const {
    if (m_muted) {
        instance->setVolume(0.0f);
    } else {
        instance->setVolume(m_volume);
    }
    instance->setAttributes(m_position, m_rotation, m_velocity);
}

} // namespace audio
} // namespace aderite
