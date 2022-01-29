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
    if (m_audioClip == nullptr) {
        return;
    }

    if (m_instance == nullptr) {
        // Create audio instance
        m_instance = ::aderite::Engine::getAudioController()->createAudioInstance(m_audioClip);

        // Set properties
        if (m_muted) {
            m_instance->setVolume(0.0f);
        } else {
            m_instance->setVolume(m_volume);
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

void AudioSource::setAudioClip(asset::AudioAsset* clip) {
    m_audioClip = clip;

    if (m_audioClip == nullptr) {
        // Free FMOD instance
        this->stop();
    }
}

asset::AudioAsset* AudioSource::getAudioClip() const {
    return m_audioClip;
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

void AudioSource::mute() {
    LOG_TRACE("[Audio] Muting source {0}", m_gObject->getName());
    m_muted = true;

    if (m_instance == nullptr) {
        return;
    }

    m_instance->setVolume(0.0f);
}

void AudioSource::unmute() {
    LOG_TRACE("[Audio] Unmuting source {0}", m_gObject->getName());
    m_muted = false;

    if (m_instance == nullptr) {
        return;
    }

    m_instance->setVolume(m_volume);
}

float AudioSource::getVolume() const {
    return m_volume;
}

void AudioSource::setVolume(const float volume) {
    LOG_TRACE("[Audio] Changing source {0} volume from {1} to {2}", m_gObject->getName(), m_volume, volume);
    m_volume = volume;

    if (m_instance == nullptr) {
        return;
    }

    if (!m_muted) {
        m_instance->setVolume(volume);
    } else {
        m_instance->setVolume(0.0f);
    }
}

bool AudioSource::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "AudioSource" << YAML::BeginMap;
    emitter << YAML::Key << "Volume" << YAML::Value << m_volume;
    if (m_audioClip) {
        emitter << YAML::Key << "Clip" << YAML::Value << m_audioClip->getHandle();
    }
    emitter << YAML::EndMap;
    return true;
}

bool AudioSource::deserialize(io::Serializer* serializer, const YAML::Node& data) {
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

} // namespace audio
} // namespace aderite
