#include "Bank.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/audio/AudioEvent.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

Bank::~Bank() {
	for (AudioEvent* e : m_events) {
		e->unload();
		delete e;
	}

	if (m_bank->unload() != FMOD_OK) {
		LOG_WARN("Failed to unload bank {0}", m_name);
	}
}

std::string Bank::getName() const {
	return m_name;
}

const std::vector<AudioEvent*>& Bank::getEvents() const {
	return m_events;
}

AudioEvent* Bank::getEvent(const std::string& name) {
	auto it = std::find_if(m_events.begin(), m_events.end(), [&](AudioEvent* e) {
		return e->getName() == name;
	});

	if (it == m_events.end()) {
		LOG_WARN("Failed to find event {0} in bank {1}", m_name, name);
		return nullptr;
	}

	return (*it);
}

void Bank::unloadData() {
	if (m_bank->unloadSampleData() != FMOD_OK) {
		LOG_WARN("Failed to unload sample data for bank {0}", m_name);
	}
}

Bank::Bank(FMOD::Studio::System* system, const std::string& name)
	: m_name(name)
{
	std::filesystem::path resDir = ::aderite::Engine::getAssetManager()->getResDir();

	if (system->loadBankFile((resDir / (name)).string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_bank)) {
		LOG_ERROR("Failed to load bank file");
		return;
	}

	// Load events
	int count = 0;
	if (m_bank->getEventCount(&count) != FMOD_OK) {
		LOG_ERROR("Failed to get event count for bank {0}", m_name);
		return;
	}
	m_events.resize(count);

	std::vector<FMOD::Studio::EventDescription*> rawEvents;
	rawEvents.resize(count);

	if (m_bank->getEventList(rawEvents.data(), rawEvents.size(), nullptr) != FMOD_OK) {
		LOG_ERROR("Failed to get event list from bank {0}", m_name);
		return;
	}

	for (int i = 0; i < count; i++) {
		m_events[i] = new AudioEvent(rawEvents[i]);
	}
}



ADERITE_AUDIO_NAMESPACE_END
