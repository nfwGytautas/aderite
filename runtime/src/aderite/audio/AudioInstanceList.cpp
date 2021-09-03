#include "AudioInstanceList.hpp"

#include "aderite/audio/AudioInstance.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

AudioInstanceList::~AudioInstanceList() {
	for (AudioInstance* ai : m_instances) {
		delete ai;
	}
}

void AudioInstanceList::addInstance(AudioInstance* instance) {
	m_instances.push_back(instance);
}

void AudioInstanceList::removeInstance(AudioInstance* instance) {
	auto& it = std::find(m_instances.begin(), m_instances.end(), instance);

	if (it != m_instances.end()) {
		delete* it;
		m_instances.erase(it);
	}
}

bool AudioInstanceList::serialize(YAML::Emitter& out) {
	for (AudioInstance* ai : m_instances) {
		out << YAML::BeginMap;
		ai->serialize(out);
		out << YAML::EndMap;
	}

	return true;
}

bool AudioInstanceList::deserialize(YAML::Node& data) {
	if (!data.IsSequence()) {
		return false;
	}

	for (auto instanceEntry : data) {
		// Load it
		AudioInstance* instance = new AudioInstance();
		if (!instance->deserialize(instanceEntry)) {
			return false;
		}
		addInstance(instance);
	}

	return true;
}

ADERITE_AUDIO_NAMESPACE_END
