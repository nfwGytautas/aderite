#include "AudioListenerData.hpp"

namespace aderite {
namespace audio {

void AudioListenerData::disable() {
    m_enabled = false;
}

void AudioListenerData::enable() {
    m_enabled = true;
}

bool AudioListenerData::isEnabled() const {
    return m_enabled;
}

bool AudioListenerData::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "AudioListener" << YAML::BeginMap;
    emitter << YAML::Key << "Enabled" << YAML::Value << m_enabled;
    emitter << YAML::EndMap;
    return true;
}

bool AudioListenerData::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& audioListener = data["AudioListener"];
    if (!audioListener || audioListener.IsNull()) {
        return false;
    }

    m_enabled = audioListener["Enabled"].as<bool>();
    return true;
}

AudioListenerData& AudioListenerData::operator=(const AudioListenerData& other) {
    m_enabled = other.m_enabled;
    return *this;
}

} // namespace audio
} // namespace aderite
