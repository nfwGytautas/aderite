#include "AudioSource.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace audio {

AudioSource::AudioSource() : m_name(utility::generateString(12)) {}

AudioSource::~AudioSource() {}

void AudioSource::update() const {
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
    LOG_TRACE("[Audio] Muting source {0}", m_name);
    m_muted = true;
}

void AudioSource::unmute() {
    LOG_TRACE("[Audio] Unmuting source {0}", m_name);
    m_muted = false;
}

void AudioSource::stop() {
    LOG_TRACE("[Audio] Stopping source {0}", m_name);

    for (AudioInstance* instance : m_instances) {
        instance->stop();
    }

    for (AudioInstance* oneshot : m_oneshots) {
        oneshot->stop();
    }
}

void AudioSource::setName(const std::string& name) {
    LOG_TRACE("[Audio] Renaming source {0} to {1}", m_name, name);
    m_name = name;
}

std::string AudioSource::getName() const {
    return m_name;
}

float AudioSource::getVolume() const {
    return m_volume;
}

glm::vec3 AudioSource::getPosition() const {
    return m_position;
}

glm::quat AudioSource::getRotation() const {
    return m_rotation;
}

glm::vec3 AudioSource::getVelocity() const {
    return m_velocity;
}

void AudioSource::setVolume(const float volume) {
    LOG_TRACE("[Audio] Changing source {0} volume from {1} to {2}", m_name, m_volume, volume);
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
    emitter << YAML::Key << "Name" << YAML::Value << m_name;
    emitter << YAML::Key << "Volume" << YAML::Value << m_volume;
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_rotation;
    emitter << YAML::Key << "Velocity" << YAML::Value << m_velocity;
    return true;
}

bool AudioSource::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Name"].as<std::string>();
    m_volume = data["Volume"].as<float>();
    m_position = data["Position"].as<glm::vec3>();
    m_rotation = data["Rotation"].as<glm::quat>();
    m_velocity = data["Velocity"].as<glm::vec3>();

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
