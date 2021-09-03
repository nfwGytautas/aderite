#include "AudioEvent.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

std::string AudioEvent::getName() const {
	return m_name;
}

void AudioEvent::load() {
	if (!isLoaded()) {
		m_description->loadSampleData();
	}
}

void AudioEvent::unload() {
	if (isLoaded()) {
		m_description->unloadSampleData();
	}
}

bool AudioEvent::isLoaded() const {
	FMOD_STUDIO_LOADING_STATE lstate;
	if (m_description->getSampleLoadingState(&lstate) != FMOD_OK) {
		LOG_WARN("Failed to get sample loading state");
		return false;
	}

	return lstate & FMOD_STUDIO_LOADING_STATE_LOADED;
}

FMOD::Studio::EventDescription* AudioEvent::getFmodDescription() const {
	return m_description;
}

AudioEvent::AudioEvent(FMOD::Studio::EventDescription* rawDesc)
	: m_description(rawDesc)
{
	int length = 0;
	if (m_description->getPath(nullptr, 0, &length) != FMOD_OK) {
		LOG_ERROR("Failed to get event name length");
		return;
	}
	m_name.resize(length);

	if (m_description->getPath(m_name.data(), length, nullptr) != FMOD_OK) {
		LOG_ERROR("Failed to get event name");
		return;
	}
}

ADERITE_AUDIO_NAMESPACE_END
