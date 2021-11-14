#include "AudioAsset.hpp"

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

AudioAsset::~AudioAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());
}

void AudioAsset::setEventName(const std::string& name) {
    LOG_TRACE("[Asset] Renaming {0}, event from {1} to {2}", this->getHandle(), m_name, name);
    m_name = name;
}

std::string AudioAsset::getEventName() const {
    return m_name;
}

void AudioAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getHandle());
}

void AudioAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getHandle());
}

bool AudioAsset::needsLoading() {
    return false;
}

reflection::Type AudioAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::AUDIO);
}

bool AudioAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Event" << YAML::Value << m_name;
    return true;
}

bool AudioAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Event"].as<std::string>();
    return true;
}

} // namespace asset
} // namespace aderite
