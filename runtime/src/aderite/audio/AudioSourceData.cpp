#include "AudioSourceData.hpp"

namespace aderite {
namespace audio {

AudioSourceData::~AudioSourceData() {}

void AudioSourceData::mute() {
    m_muted = true;
}

void AudioSourceData::unmute() {
    m_muted = false;
}

bool AudioSourceData::isMuted() const {
    return m_muted;
}

float AudioSourceData::getVolume() const {
    return m_volume;
}

void AudioSourceData::setVolume(const float volume) {
    m_volume = volume;
}

bool AudioSourceData::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "AudioSource" << YAML::BeginMap;
    emitter << YAML::Key << "Volume" << YAML::Value << m_volume;
    emitter << YAML::EndMap;
    return true;
}

bool AudioSourceData::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& audioSource = data["AudioSource"];
    if (!audioSource || audioSource.IsNull()) {
        return false;
    }

    this->setVolume(audioSource["Volume"].as<float>());
    return true;
}

AudioSourceData& AudioSourceData::operator=(const AudioSourceData& other) {
    m_volume = other.m_volume;
    m_muted = other.m_muted;
    return *this;
}

} // namespace audio
} // namespace aderite
