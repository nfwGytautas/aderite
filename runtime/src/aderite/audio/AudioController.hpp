#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <fmod_studio.hpp>
#include "aderite/utility/Macros.hpp"
#include "aderite/audio/Forward.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

/**
 * @brief Class used to control audio API FMOD and provide a functionality to play audio in aderite
*/
class AudioController {
public:
	/**
	 * @brief Initializes the audio controller
	*/
	bool init();

	/**
	 * @brief Shutdown the audio controller
	*/
	void shutdown();

	/**
	 * @brief Updates all audio components of current scene
	*/
	void update();

	/**
	 * @brief Loads the master bank (invoked automatically by AssetManager)
	 * @param dir Path to directory where master bank (strings.bank should be in the same directory) exists
	*/
	void loadMasterBank(const std::filesystem::path& dir);

	/**
	 * @brief Returns true if master banks loaded, false otherwise
	*/
	bool masterBanksLoaded() const;

	/**
	 * @brief Return true if the AudioController has the bank loaded
	*/
	bool has(const std::string& name);

	/**
	 * @brief Reads the bank information and returns it
	 * @param path Name of the bank
	 * @return Bank instance or nullptr if failed
	*/
	Bank* readBank(const std::string& path);

	/**
	 * @brief Loads a bank and returns it
	 * @param name Name of the bank
	 * @return Bank instance
	*/
	Bank* getOrRead(const std::string& name);

	/**
	 * @brief Returns bank by name
	 * @param name Name of the bank
	 * @return Bank instance if bank with given name was loaded, nullptr otherwise
	*/
	Bank* getBank(const std::string& name);

	/**
	 * @brief Unload bank by name
	 * @param name Name of the bank
	*/
	void unloadBank(const std::string& name);

	/**
	 * @brief Unload bank by instance
	 * @param bank Instance to unload
	*/
	void unloadBank(Bank* bank);

	/**
	 * @brief If true all sounds will be muted (not stopped)
	*/
	void setMute(bool value);

	/**
	 * @brief If true the audio controller will stop all audio instances and won't play any new ones
	 * until false is passed. Every time this cycle is done play on start instances will start playing. 
	*/
	void disable(bool value);

	/**
	 * @brief Unloads all audio banks
	*/
	void unloadAll();

	auto begin() {
		return m_banks.begin();
	}

	auto begin() const {
		return m_banks.begin();
	}

	auto end() {
		return m_banks.end();
	}

	auto end() const {
		return m_banks.end();
	}
private:
	AudioController() {}
	friend class Engine;

private:
	FMOD::Studio::System* m_fmodSystem = nullptr;
	FMOD::Studio::Bank* m_masterBank = nullptr;
	FMOD::Studio::Bank* m_stringBank = nullptr;

	// Loaded banks
	std::vector<Bank*> m_banks;

	bool m_mute = false;
	bool m_disabled = false;
	bool m_wasDisabled = true;
};

ADERITE_AUDIO_NAMESPACE_END
