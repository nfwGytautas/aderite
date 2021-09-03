#pragma once

#include <string>
#include <fmod_studio.hpp>
#include "aderite/utility/Macros.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

/**
 * @brief Audio event, wrapper for FMOD EventDescription
*/
class AudioEvent {
public:
	/**
	 * @brief Returns the name of the audio event
	 * @return Name of the event
	*/
	std::string getName() const;

	/**
	 * @brief Loads data of the event
	*/
	void load();

	/**
	 * @brief Unloads data of the event
	*/
	void unload();

	/**
	 * @brief Returns true if the event is loaded or not
	 * @return True if loaded, false otherwise
	*/
	bool isLoaded() const;

	/**
	 * @brief Returns the FMOD event description of the object
	*/
	FMOD::Studio::EventDescription* getFmodDescription() const;
private:
	AudioEvent(FMOD::Studio::EventDescription* rawDesc);
	friend class Bank;

private:
	FMOD::Studio::EventDescription* m_description = nullptr;
	std::string m_name = "";
};

ADERITE_AUDIO_NAMESPACE_END
