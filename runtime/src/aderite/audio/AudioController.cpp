#include "AudioController.hpp"

#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <fmod_studio_common.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

namespace aderite {
namespace audio {

constexpr int c_MaxChannels = 512;

#ifdef _DEBUG
// Enable memory tracking for debug
constexpr FMOD_STUDIO_INITFLAGS c_StudioInitFlags = FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_MEMORY_TRACKING |
                                                    FMOD_STUDIO_INIT_LIVEUPDATE;
constexpr FMOD_INITFLAGS c_InitFlags = FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE | FMOD_INIT_MEMORY_TRACKING;

// constexpr int c_MemoryEnable = FMOD_DEBUG_TYPE_MEMORY;
constexpr int c_MemoryEnable = 0;

FMOD_RESULT debugCallback(FMOD_DEBUG_FLAGS flags, const char* file, int line, const char* func, const char* message) {
    // Remove extra '\n'
    auto msg = std::string(message);
    msg.pop_back();

    if (flags & FMOD_DEBUG_LEVEL_ERROR) {
        LOG_ERROR("[Audio] FMOD {0} in function {1}", msg, func);
    } else if (flags & FMOD_DEBUG_LEVEL_WARNING) {
        LOG_WARN("[Audio] FMOD {0} in function {1}", msg, func);
    } else if (flags & (FMOD_DEBUG_TYPE_TRACE | FMOD_DEBUG_LEVEL_LOG)) {
        LOG_TRACE("[Audio] FMOD {0}", msg);
    } else {
        if (flags & FMOD_DEBUG_TYPE_MEMORY) {
            LOG_TRACE("[Audio] FMOD (memory) {0}", msg);
        }
    }

    return FMOD_OK;
}

#else
constexpr FMOD_STUDIO_INITFLAGS c_StudioInitFlags = FMOD_STUDIO_INIT_NORMAL;
constexpr FMOD_INITFLAGS c_InitFlags = FMOD_STUDIO_NORMAL;
#endif

bool AudioController::init() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Audio] Initializing audio controller");

    // Debug
#ifdef _DEBUG
    LOG_TRACE("[Audio] Initialzing FMOD debug callbacks");
    if (FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG | FMOD_DEBUG_TYPE_TRACE | c_MemoryEnable, FMOD_DEBUG_MODE_CALLBACK, debugCallback,
                               nullptr) != FMOD_OK) {
        LOG_ERROR("[Audio] Failed to initialize FMOD debug callback for messages");
        return false;
    }
    LOG_INFO("[Audio] FMOD debug callbacks initialized");
#endif

    // Create
    LOG_TRACE("[Audio] Creating FMOD system");
    if (FMOD::Studio::System::create(&m_fmodSystem) != FMOD_OK) {
        LOG_ERROR("[Audio] Failed to create FMOD studio system");
        return false;
    }
    LOG_INFO("[Audio] FMOD system created");

    // Initialize
    LOG_TRACE("[Audio] Initializing FMOD system");
    if (m_fmodSystem->initialize(c_MaxChannels, c_StudioInitFlags, c_InitFlags, nullptr) != FMOD_OK) {
        LOG_ERROR("[Audio] Failed to initialized FMOD studio system");
        return false;
    }
    LOG_INFO("[Audio] FMOD system initialized");

    // Aderite only supports single listener
    LOG_TRACE("[Audio] Setting listener count");
    if (m_fmodSystem->setNumListeners(1) != FMOD_OK) {
        LOG_ERROR("[Audio] Failed to set number of listeners to 1, something is wrong with audio controller, aborting");
        return false;
    }
    LOG_INFO("[Audio] Listener count set");

    LOG_INFO("[Audio] Audio controller initialized");
    return true;
}

void AudioController::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Audio] Shutting down audio controller");
    m_fmodSystem->release();
    LOG_INFO("[Audio] Audio controller shutdown");
}

void AudioController::update() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (currentScene != nullptr) {
        // Configure listener
        bool thisFrameMute = false;
        int enabledListenerCount = 0;

        auto& listeners = currentScene->getAudioListeners();

        // If no listeners no need to update anything
        for (auto listener : listeners) {
            if (listener->isEnabled()) {
                if (enabledListenerCount > 1) {
                    LOG_WARN("[Audio] Multiple enabled listeners, the last one will be used as the listener");
                }

                listener->update();
                enabledListenerCount++;
            }
        }

        // If no active listeners, mute
        if (enabledListenerCount > 0) {
            // Configure sources
            for (AudioSource* source : currentScene->getAudioSources()) {
                source->update();
            }
        }
    }

    if (m_fmodSystem->update() != FMOD_OK) {
        LOG_WARN("[Audio] Failed to update audio controller");
    }
}

void AudioController::loadMasterBank() {
    LOG_TRACE("[Audio] Loading master banks");
    io::DataChunk masterChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::MasterAudioBank);
    io::DataChunk stringsChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::StringsAudioBank);

    if (masterChunk.Data.size() == 0 || stringsChunk.Data.size() == 0) {
        LOG_WARN("[Audio] Ignored loadMasterBank call, cause no master or strings bank was found");
        return;
    }

    // Unload
    if (m_stringBank != nullptr) {
        LOG_TRACE("[Audio] Unloading strings bank");
        m_stringBank->unload();
    }

    if (m_masterBank != nullptr) {
        LOG_TRACE("[Audio] Unloading master bank");
        m_masterBank->unload();
    }

    m_stringBank = nullptr;
    m_masterBank = nullptr;

    LOG_TRACE("[Audio] Loading banks from memory");

    // Load
    FMOD_RESULT result = m_fmodSystem->loadBankMemory(reinterpret_cast<const char*>(stringsChunk.Data.data()), stringsChunk.Data.size(),
                                                      FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_stringBank);

    ADERITE_DYNAMIC_ASSERT(result == FMOD_OK, "Failed to load strings bank");

    // Load
    result = m_fmodSystem->loadBankMemory(reinterpret_cast<const char*>(masterChunk.Data.data()), masterChunk.Data.size(),
                                          FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);

    ADERITE_DYNAMIC_ASSERT(result == FMOD_OK, "Failed to load master bank");

    LOG_TRACE("[Audio] Querying strings");
    int strings = 0;
    ADERITE_DYNAMIC_ASSERT(m_stringBank->getStringCount(&strings) == FMOD_OK, "Failed to query string count");

    LOG_TRACE("[Audio] Querying events");
    constexpr size_t c_pathSize = 100;
    std::string pathHolder;
    pathHolder.resize(c_pathSize);
    for (int i = 0; i < strings; i++) {
        // TODO: Length overflow check
        int length = 0;
        ADERITE_DYNAMIC_ASSERT(m_stringBank->getStringInfo(i, nullptr, pathHolder.data(), c_pathSize, &length) == FMOD_OK, "Failed to "
                                                                                                                           "query string "
                                                                                                                           "count");

        LOG_TRACE("[Audio] Found {0}", pathHolder);
        if (pathHolder.find("event:/") != std::string::npos) {
            m_knownEvents.push_back(pathHolder);
        }
    }

    LOG_INFO("[Audio] Banks loaded");
}

bool AudioController::masterBanksLoaded() const {
    return m_masterBank != nullptr && m_stringBank != nullptr;
}

AudioInstance* aderite::audio::AudioController::createAudioInstance(const std::string name) {
    FMOD::Studio::EventDescription* desc = nullptr;
    FMOD::Studio::EventInstance* instance;
    FMOD_STUDIO_LOADING_STATE loadState;

    LOG_TRACE("[Audio] Creating AudioInstance for {0}", name);
    ADERITE_DYNAMIC_ASSERT(m_fmodSystem->getEvent(name.c_str(), &desc) == FMOD_OK, "Failed to get event");
    ADERITE_DYNAMIC_ASSERT(desc->createInstance(&instance) == FMOD_OK, "Failed to create event instance");

    desc->getSampleLoadingState(&loadState);
    if (loadState == FMOD_STUDIO_LOADING_STATE_UNLOADED) {
        ADERITE_DYNAMIC_ASSERT(desc->loadSampleData() == FMOD_OK, "Failed to load event sample data");
    }

    AudioInstance* aderiteInstance = new AudioInstance(instance);
    LOG_INFO("[Audio] {0} instance created", name);
    return aderiteInstance;
}

void AudioController::setMute(bool value) {
    LOG_TRACE("[Audio] Changing mute state to {0}", value);

    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (currentScene == nullptr) {
        return;
    }

    for (AudioSource* source : currentScene->getAudioSources()) {
        if (value) {
            source->mute();
        } else {
            source->unmute();
        }
    }
}

void AudioController::disable(bool value) {
    LOG_TRACE("[Audio] Changing enabled state to {0}", value);

    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (currentScene == nullptr) {
        return;
    }

    for (AudioSource* source : currentScene->getAudioSources()) {
        if (value) {
            source->stop();
        } else {
            // TODO: Play?
        }
    }
}

const std::vector<std::string>& aderite::audio::AudioController::getKnownEvents() const {
    return m_knownEvents;
}

} // namespace audio
} // namespace aderite
