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
    if (!m_enabled) {
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

void AudioListener::disable() {
    LOG_TRACE("[Audio] Disabling listener {0}", m_gObject->getName());
    m_enabled = false;
}

void AudioListener::enable() {
    LOG_TRACE("[Audio] Enabling listener {0}", m_gObject->getName());
    m_enabled = true;
}

bool AudioListener::isEnabled() const {
    return m_enabled;
}

bool AudioListener::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "AudioListener" << YAML::BeginMap;
    emitter << YAML::Key << "Enabled" << YAML::Value << m_enabled;
    emitter << YAML::EndMap;
    return true;
}

bool AudioListener::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& audioListener = data["AudioListener"];
    if (!audioListener || audioListener.IsNull()) {
        return false;
    }

    m_enabled = audioListener["Enabled"].as<bool>();
    return true;
}

} // namespace audio
} // namespace aderite
