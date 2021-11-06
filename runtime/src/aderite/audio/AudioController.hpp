#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmod_studio.hpp>

#include "aderite/audio/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
class Engine;

namespace audio {

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
     * @brief Loads the master bank (invoked automatically or by hand through the editor)
     */
    void loadMasterBank();

    /**
     * @brief Returns true if master banks loaded, false otherwise
     */
    bool masterBanksLoaded() const;

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
     * @brief Returns vector of known events
     */
    const std::vector<std::string>& getKnownEvents() const;

    /**
     * @brief Creates an audio source object and returns it
     */
    AudioSource* createSource();

    /**
     * @brief Adds a source to controller
     * @param source Source to add
     */
    void addSource(AudioSource* source);

    /**
     * @brief Returns audio source associated with the specified handle
     * @param handle Handle of the source
     * @return AudioSource instance or nullptr if audio controller doesn't have the source
    */
    AudioSource* getSource(SourceHandle handle);

    /**
     * @brief Creates and returns an audio instance
     * @param name Name of the audio
     * @return AudioIntance object
     */
    AudioInstance* createAudioInstance(const std::string name);

    /**
     * @brief Returns audio sources currently registered in the audio controller
     */
    const std::vector<AudioSource*>& getAudioSources() const {
        return m_sources;
    }

private:
    AudioController() {}
    friend Engine;

    /**
     * @brief Unload all data
     */
    void unloadAll();

private:
    FMOD::Studio::System* m_fmodSystem = nullptr;
    FMOD::Studio::Bank* m_masterBank = nullptr;
    FMOD::Studio::Bank* m_stringBank = nullptr;

    // Loaded banks
    std::vector<std::string> m_knownEvents;

    std::vector<AudioSource*> m_sources;
};

} // namespace audio
} // namespace aderite
