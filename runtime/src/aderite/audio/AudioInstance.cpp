#include "AudioInstance.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/Bank.hpp"
#include "aderite/audio/AudioEvent.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

AudioInstance::~AudioInstance() {
	reset();
}

void AudioInstance::start() {
	if (m_instance != nullptr) {
		if (m_instance->start() != FMOD_OK) {
			LOG_WARN("Failed to start playing {0} instance", m_event->getName());
		}
	}
}

void AudioInstance::stop() {
	if (m_instance != nullptr) {
		if (m_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE) != FMOD_OK) {
			LOG_WARN("Failed to stop playing {0} instance", m_event->getName());
		}
	}
}

void AudioInstance::fadeout() {
	if (m_instance != nullptr) {
		if (m_instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT) != FMOD_OK) {
			LOG_WARN("Failed to stop (fadeout) playing {0} instance", m_event->getName());
		}
	}
}

void AudioInstance::init() {
	if (m_event == nullptr) {
		LOG_WARN("Tried to initialize instance of nullptr event");
		return;
	}

	if (m_event->getFmodDescription()->createInstance(&m_instance) != FMOD_OK) {
		LOG_WARN("Failed to create {0} instance", m_event->getName());
	}
}

bool AudioInstance::canBeInitialized() const {
	return !isInitialized() && m_event != nullptr && m_bank != nullptr;
}

bool AudioInstance::isInitialized() const {
	return m_instance != nullptr;
}

bool AudioInstance::getPlayOnStart() const {
	return m_playOnStart;
}

void AudioInstance::setPlayOnStart(bool value) {
	m_playOnStart = value;
}

FMOD::Studio::EventInstance* AudioInstance::getFmodInstance() const {
	return m_instance;
}

void AudioInstance::setBank(Bank* bank) {
	reset();

	m_bank = bank;
	m_event = nullptr;
}

void AudioInstance::setEvent(AudioEvent* e) {
	reset();

	m_event = e;
}

Bank* AudioInstance::getBank() const {
	return m_bank;
}

AudioEvent* AudioInstance::getEvent() const {
	return m_event;
}

bool AudioInstance::serialize(YAML::Emitter& out) {
	// General
	if (m_bank != nullptr) {
		out << YAML::Key << "Bank" << YAML::Value << m_bank->getName();

		if (m_event != nullptr) {
			out << YAML::Key << "Event" << YAML::Value << m_event->getName();
		}
	}
	
	// Shared
	out << YAML::Key << "PlayOnStart" << YAML::Value << m_playOnStart;

	return true;
}

bool AudioInstance::deserialize(YAML::Node& data) {
	if (data["Bank"]) {
		std::string bankName = data["Bank"].as<std::string>();

		m_bank = ::aderite::Engine::getAudioController()->getOrRead(bankName);

		if (m_bank == nullptr) {
			LOG_ERROR("Failed to read audio instance bank");
			return false;
		}

		if (data["Event"]) {
			std::string eventName = data["Event"].as<std::string>();
			m_event = m_bank->getEvent(eventName);

			if (m_event == nullptr) {
				LOG_ERROR("Failed to read event from bank");
				return false;
			}
		}
	}

	// Shared
	m_playOnStart = data["PlayOnStart"].as<bool>();

	return true;
}

void AudioInstance::reset() {
	if (m_instance != nullptr) {
		this->stop();
		m_instance->release();
		m_instance = nullptr;
	}
}

ADERITE_AUDIO_NAMESPACE_END
