#include "AudioAsset.hpp"

namespace aderite {
namespace asset {

AudioAsset::~AudioAsset() {}

void AudioAsset::setEventName(const std::string& name) {
    m_name = name;
}

std::string AudioAsset::getEventName() const {
    return m_name;
}

void AudioAsset::load(const io::Loader* loader) {}

void AudioAsset::unload() {}

bool AudioAsset::needsLoading() {
    return false;
}

reflection::Type AudioAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::AUDIO);
}

bool AudioAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "Event" << YAML::Value << m_name;
    return true;
}

bool AudioAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Event"].as<std::string>();
    return true;
}

} // namespace asset
} // namespace aderite
