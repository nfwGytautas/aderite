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
     * @brief Creates and returns an audio instance
     * @param name Name of the audio
     * @return AudioIntance object
     */
    AudioInstance* createAudioInstance(const std::string name);

    /**
     * @brief Returns the fmod system instance of aderite
    */
    FMOD::Studio::System* getFmodSystem() const {
        return m_fmodSystem;
    }

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
