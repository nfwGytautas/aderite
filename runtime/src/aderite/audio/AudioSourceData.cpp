#include "AudioSourceData.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/AudioAsset.hpp"

namespace aderite {
namespace audio {

AudioSourceData::~AudioSourceData() {
    if (m_audioClip != nullptr) {
        m_audioClip->release();
    }
}

void AudioSourceData::setAudioClip(asset::AudioAsset* clip) {
    if (m_audioClip != nullptr) {
        m_audioClip->release();
    }

    if (clip != nullptr) {
        clip->acquire();
    }

    m_audioClip = clip;
}

asset::AudioAsset* AudioSourceData::getAudioClip() const {
    return m_audioClip;
}

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
    if (m_audioClip) {
        emitter << YAML::Key << "Clip" << YAML::Value << m_audioClip->getHandle();
    }
    emitter << YAML::EndMap;
    return true;
}

bool AudioSourceData::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& audioSource = data["AudioSource"];
    if (!audioSource || audioSource.IsNull()) {
        return false;
    }

    this->setVolume(audioSource["Volume"].as<float>());

    if (audioSource["Clip"]) {
        const io::SerializableHandle handle = audioSource["Clip"].as<io::SerializableHandle>();
        this->setAudioClip(static_cast<asset::AudioAsset*>(::aderite::Engine::getAssetManager()->get(handle)));
    }

    return true;
}

AudioSourceData& AudioSourceData::operator=(const AudioSourceData& other) {
    this->setAudioClip(other.getAudioClip());
    m_volume = other.m_volume;
    m_muted = other.m_muted;
    return *this;
}

} // namespace audio
} // namespace aderite
