#include "AudioListener.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace audio {

AudioListener::AudioListener() : m_name(utility::generateString(12)) {}

AudioListener::~AudioListener() {}

void AudioListener::update() {
    FMOD_3D_ATTRIBUTES listener3dAttributes = {};
    listener3dAttributes.position = {m_position.x, m_position.y, m_position.z};
    listener3dAttributes.velocity = {m_velocity.x, m_velocity.y, m_velocity.z};

    // TODO: Rotation
    listener3dAttributes.up = {0.0f, 1.0f, 0.0f};
    listener3dAttributes.forward = {0.0f, 0.0f, 1.0f};

    // TODO: Attenuation

    // Set attributes on FMOD instance
    ::aderite::Engine::getAudioController()->getFmodSystem()->setListenerAttributes(0, &listener3dAttributes, nullptr);
}

void AudioListener::disable() {
    m_enabled = false;
}

void AudioListener::enable() {
    m_enabled = true;
}

void AudioListener::setName(const std::string& name) {
    m_name = name;
}

void AudioListener::setPosition(const glm::vec3& position) {
    m_position = position;
}

void AudioListener::setRotation(const glm::quat& rotation) {
    m_rotation = rotation;
}

void AudioListener::setVelocity(const glm::vec3& velocity) {
    m_velocity = velocity;
}

reflection::Type AudioListener::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::AUDIO_LISTENER);
}

bool AudioListener::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Name" << YAML::Value << m_name;
    emitter << YAML::Key << "Enabled" << YAML::Value << m_enabled;
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_rotation;
    emitter << YAML::Key << "Velocity" << YAML::Value << m_velocity;
    return true;
}

bool AudioListener::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Name"].as<std::string>();
    m_enabled = data["Enabled"].as<bool>();
    m_position = data["Position"].as<glm::vec3>();
    m_rotation = data["Rotation"].as<glm::quat>();
    m_velocity = data["Velocity"].as<glm::vec3>();

    return true;
}

} // namespace audio
} // namespace aderite
