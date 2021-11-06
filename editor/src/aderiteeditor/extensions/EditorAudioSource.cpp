#include "EditorAudioSource.hpp"

#include "aderite/utility/Random.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace audio {

EditorAudioSource::EditorAudioSource() : m_name(utility::generateString(16)) {}

std::string EditorAudioSource::getName() const {
    return m_name;
}

void EditorAudioSource::setName(const std::string& name) {
    // TODO: Check if unique
    m_name = name;
}

bool EditorAudioSource::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!AudioSource::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "Name" << YAML::Value << m_name;

    return true;
}

bool EditorAudioSource::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!AudioSource::deserialize(serializer, data)) {
        return false;
    }

    m_name = data["Name"].as<std::string>();

    return true;
}

} // namespace audio
} // namespace aderite
