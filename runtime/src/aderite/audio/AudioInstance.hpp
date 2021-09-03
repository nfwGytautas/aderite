#pragma once

#include <fmod_studio.hpp>
#include "aderite/utility/Macros.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

/**
 * @brief Audio instance class used to wrap around FMOD EventInstance
*/
class AudioInstance : public interfaces::ISerializable {
public:
	~AudioInstance();

	/**
	 * @brief Starts playing audio
	*/
	void start();

	/**
	 * @brief Stops playing audio
	*/
	void stop();

	/**
	 * @brief Fades the audio out and stops playing
	*/
	void fadeout();

	/**
	 * @brief Initializes instance
	*/
	void init();

	/**
	 * @brief Returns true if the instance can be initialized, false if not or already initialized
	*/
	bool canBeInitialized() const;

	/**
	 * @brief Returns true if the instance is initialized, false otherwise
	*/
	bool isInitialized() const;

	/**
	 * @brief Returns true if asset tagged to play on start, false otherwise
	*/
	bool getPlayOnStart() const;

	/**
	 * @brief Sets the play on start value
	*/
	void setPlayOnStart(bool value);

	/**
	 * @brief Returns the FMOD instance of the event
	*/
	FMOD::Studio::EventInstance* getFmodInstance() const;

	/**
	 * @brief Sets the bank of the audio instance removes current event
	 * @param bank New bank instance
	*/
	void setBank(Bank* bank);

	/**
	 * @brief Sets the audio event from bank
	 * @param e Event to set
	*/
	void setEvent(AudioEvent* e);

	/**
	 * @brief Returns the Bank instance
	*/
	Bank* getBank() const;

	/**
	 * @brief Returns the event description instance
	*/
	AudioEvent* getEvent() const;

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

private:
	void reset();

private:
	audio::Bank* m_bank = nullptr;
	audio::AudioEvent* m_event = nullptr;
	FMOD::Studio::EventInstance* m_instance = nullptr;

	// Properties
	bool m_playOnStart = false;
};

ADERITE_AUDIO_NAMESPACE_END
