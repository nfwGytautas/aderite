#pragma once

#include <string>
#include <vector>
#include <fmod_studio.hpp>
#include "aderite/utility/Macros.hpp"
#include "aderite/audio/Forward.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

/**
 * @brief Class used to wrap FMOD bank
*/
class Bank {
public:
	virtual ~Bank();

	/**
	 * @brief Returns the name of the bank
	*/
	std::string getName() const;

	/**
	 * @brief Returns events contained in the bank
	*/
	const std::vector<AudioEvent*>& getEvents() const;

	/**
	 * @brief Returns the event by name
	 * @param name Name of the event
	 * @return AudioEvent instance
	*/
	AudioEvent* getEvent(const std::string& name);

	/**
	 * @brief Unloads all data of the bank, to completely unload the bank
	 * call AudioController::unloadBank
	*/
	void unloadData();

	auto begin() {
		return m_events.begin();
	}

	auto begin() const {
		return m_events.begin();
	}

	auto end() {
		return m_events.end();
	}

	auto end() const {
		return m_events.end();
	}
private:
	Bank(FMOD::Studio::System* system, const std::string& name);
	friend class AudioController;
private:
	FMOD::Studio::Bank* m_bank = nullptr;
	std::vector<AudioEvent*> m_events;
	std::string m_name = "";
};

ADERITE_AUDIO_NAMESPACE_END
