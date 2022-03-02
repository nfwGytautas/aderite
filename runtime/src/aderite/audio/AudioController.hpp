#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <fmod_studio.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"

namespace aderite {
class Engine;

namespace audio {

/**
 * @brief Class used to control audio API FMOD and provide a functionality to play audio in aderite
 */
class AudioController final {
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
    void setMute(bool value) const;

    /**
     * @brief If true the audio controller will stop all audio instances and won't play any new ones
     * until false is passed. Every time this cycle is done play on start instances will start playing.
     */
    void disable(bool value) const;

    /**
     * @brief Returns vector of known events
     */
    const std::vector<std::string>& getKnownEvents() const;

    /**
     * @brief [Internal use] Creates and returns an audio instance
     * @param audioAsset AudioAsset instance to create FMOD event for
     * @return FMOD event instance object
     */
    FMOD::Studio::EventInstance* createAudioInstance(const asset::AudioAsset* audioAsset) const;

    /**
     * @brief Returns the fmod system instance of aderite
     */
    FMOD::Studio::System* getFmodSystem() const;

private:
    AudioController() {}
    friend Engine;

private:
    FMOD::Studio::System* m_fmodSystem = nullptr;
    FMOD::Studio::Bank* m_masterBank = nullptr;
    FMOD::Studio::Bank* m_stringBank = nullptr;

    // Loaded banks
    std::vector<std::string> m_knownEvents;
};

} // namespace audio
} // namespace aderite
