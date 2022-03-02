#include "AudioAsset.hpp"

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

AudioAsset::~AudioAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());
}

void AudioAsset::setEventName(const std::string& name) {
    LOG_TRACE("[Asset] Renaming {0}, event from {1} to {2}", this->getName(), m_eventName, name);
    m_eventName = name;
}

std::string AudioAsset::getEventName() const {
    return m_eventName;
}

void AudioAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());
}

void AudioAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());
}

bool AudioAsset::needsLoading() const {
    return false;
}

reflection::Type AudioAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::AUDIO);
}

bool AudioAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Event" << YAML::Value << m_eventName;
    return true;
}

bool AudioAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_eventName = data["Event"].as<std::string>();
    return true;
}

} // namespace asset
} // namespace aderite
