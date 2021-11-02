#include "AudioController.hpp"

#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <fmod_studio_common.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/utility/Log.hpp"

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
        LOG_ERROR("FMOD {0} in function {1}", msg, func);
    } else if (flags & FMOD_DEBUG_LEVEL_WARNING) {
        LOG_WARN("FMOD {0} in function {1}", msg, func);
    } else if (flags & (FMOD_DEBUG_TYPE_TRACE | FMOD_DEBUG_LEVEL_LOG)) {
        LOG_TRACE("FMOD {0}", msg);
    } else {
        if (flags & FMOD_DEBUG_TYPE_MEMORY) {
            LOG_TRACE("FMOD (memory) {0}", msg);
        }
    }

    return FMOD_OK;
}

#else
constexpr FMOD_STUDIO_INITFLAGS c_StudioInitFlags = FMOD_STUDIO_INIT_NORMAL;
constexpr FMOD_INITFLAGS c_InitFlags = FMOD_STUDIO_NORMAL;
#endif

bool AudioController::init() {
    // Debug
#ifdef _DEBUG
    if (FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG | FMOD_DEBUG_TYPE_TRACE | c_MemoryEnable, FMOD_DEBUG_MODE_CALLBACK, debugCallback,
                               nullptr) != FMOD_OK) {
        LOG_ERROR("Failed to initialize FMOD debug callback for messages");
        return false;
    }
#endif

    // Create
    if (FMOD::Studio::System::create(&m_fmodSystem) != FMOD_OK) {
        LOG_ERROR("Failed to create FMOD studio system");
        return false;
    }

    // Initialize
    if (m_fmodSystem->initialize(c_MaxChannels, c_StudioInitFlags, c_InitFlags, nullptr) != FMOD_OK) {
        LOG_ERROR("Failed to initialized FMOD studio system");
        return false;
    }

    return true;
}

void AudioController::shutdown() {
    unloadAll();
    m_fmodSystem->release();
}

void AudioController::update() {
    if (::aderite::Engine::getSceneManager()->getCurrentScene() == nullptr) {
        return;
    }

    // Configure listener
    bool thisFrameMute = false;
    int enabledListenerCount = 0;
    auto listenerGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry().group<scene::AudioListenerComponent>(
        entt::get<scene::TransformComponent>);

    // If no listeners no need to update anything
    if (listenerGroup.size() != 0) {
        if (m_fmodSystem->setNumListeners(listenerGroup.size()) != FMOD_OK) {
            LOG_WARN("Failed to set number listeners for FMOD to {0} defaulting to 1", listenerGroup.size());

            if (m_fmodSystem->setNumListeners(1) != FMOD_OK) {
                LOG_ERROR("Failed to set number of listeners to 1, something is wrong with audio controller, aborting");
                return;
            }
        }

        for (auto entity : listenerGroup) {
            auto [listener, transform] = listenerGroup.get<scene::AudioListenerComponent, scene::TransformComponent>(entity);

            if (listener.IsEnabled) {
                FMOD_3D_ATTRIBUTES listener3dAttributes = {};

                listener3dAttributes.position = {transform.Position.x, transform.Position.y, transform.Position.z};

                // TODO: Rotation
                listener3dAttributes.up = {0.0f, 1.0f, 0.0f};
                listener3dAttributes.forward = {0.0f, 0.0f, 1.0f};

                // TODO: Velocity
                listener3dAttributes.velocity = {0.0f, 0.0f, 0.0f};

                // TODO: Attenuation
                m_fmodSystem->setListenerAttributes(enabledListenerCount, &listener3dAttributes, nullptr);

                enabledListenerCount++;
            }
        }
    }

    // If no active listeners, mute
    if (enabledListenerCount == 0) {
        thisFrameMute = true;
    }

    // Configure sources
    auto audioGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry().group<scene::AudioSourceComponent>(
        entt::get<scene::TransformComponent>);
    for (auto entity : audioGroup) {
        auto [audioSource, transform] = audioGroup.get<scene::AudioSourceComponent, scene::TransformComponent>(entity);

        if (audioSource.Instance == c_InvalidHandle) {
            continue;
        }

        // Update instances
        // TODO: Error checking and handling
        FMOD::Studio::EventInstance* instance = m_instances[audioSource.Instance];
        if (m_disabled) {
            instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }

        if (m_wasDisabled && audioSource.PlayOnStart) {
            instance->start();
        }

        FMOD_3D_ATTRIBUTES source3dAttributes = {};

        // TODO: Volume
        if (m_mute || thisFrameMute) {
            instance->setVolume(0.0f);
        } else {
            instance->setVolume(1.0f);
        }

        source3dAttributes.position = {transform.Position.x, transform.Position.y, transform.Position.z};

        // TODO: Rotation
        source3dAttributes.up = {0.0f, 1.0f, 0.0f};
        source3dAttributes.forward = {0.0f, 0.0f, 1.0f};

        // TODO: Velocity
        source3dAttributes.velocity = {0.0f, 0.0f, 0.0f};

        instance->set3DAttributes(&source3dAttributes);
    }

    if (!m_disabled) {
        m_wasDisabled = false;
    }

    if (m_fmodSystem->update() != FMOD_OK) {
        LOG_WARN("Failed to update audio controller");
    }
}

void AudioController::loadMasterBank() {
    io::DataChunk masterChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::MasterAudioBank);
    io::DataChunk stringsChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::StringsAudioBank);

    if (masterChunk.Data.size() == 0 || stringsChunk.Data.size() == 0) {
        LOG_WARN("Ignored loadMasterBank call, cause no master or strings bank was found");
        return;
    }

    // Unload
    if (m_stringBank != nullptr) {
        m_stringBank->unload();
    }

    if (m_masterBank != nullptr) {
        m_masterBank->unload();
    }

    m_stringBank = nullptr;
    m_masterBank = nullptr;

    // Load
    FMOD_RESULT result = m_fmodSystem->loadBankMemory(reinterpret_cast<const char*>(stringsChunk.Data.data()), stringsChunk.Data.size(),
                                                      FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_stringBank);

    ADERITE_DYNAMIC_ASSERT(result == FMOD_OK, "Failed to load strings bank");

    // Load
    result = m_fmodSystem->loadBankMemory(reinterpret_cast<const char*>(masterChunk.Data.data()), masterChunk.Data.size(),
                                          FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);

    ADERITE_DYNAMIC_ASSERT(result == FMOD_OK, "Failed to load master bank");

    // Get all events
    this->unloadAll();

    LOG_TRACE("Querying strings");
    int strings = 0;
    ADERITE_DYNAMIC_ASSERT(m_stringBank->getStringCount(&strings) == FMOD_OK, "Failed to query string count");

    LOG_TRACE("Querying events and banks");
    constexpr size_t c_pathSize = 100;
    std::string pathHolder;
    pathHolder.reserve(c_pathSize);
    for (int i = 0; i < strings; i++) {
        ADERITE_DYNAMIC_ASSERT(m_stringBank->getStringInfo(1, nullptr, pathHolder.data(), c_pathSize, nullptr) == FMOD_OK, "Failed to "
                                                                                                                           "query string "
                                                                                                                           "count");

        LOG_TRACE("Found {0}", pathHolder);
        if (pathHolder.rfind("event:/", 0) == 0) {
            m_knownEvents.push_back(pathHolder);
        }
    }
}

bool AudioController::masterBanksLoaded() const {
    return m_masterBank != nullptr && m_stringBank != nullptr;
}

AudioInstanceId aderite::audio::AudioController::createAudioInstance(const std::string name) {
    FMOD::Studio::EventDescription* desc;
    ADERITE_DYNAMIC_ASSERT(m_fmodSystem->getEvent(name.c_str(), &desc) == FMOD_OK, "Failed to get event");
    FMOD::Studio::EventInstance* instance;
    ADERITE_DYNAMIC_ASSERT(desc->createInstance(&instance) == FMOD_OK, "Failed to create event instance");
    m_instances.push_back(instance);
    return m_instances.size() - 1;
}

void AudioController::setMute(bool value) {
    m_mute = value;
}

void AudioController::disable(bool value) {
    m_wasDisabled = m_disabled;
    m_disabled = value;
}

const std::vector<std::string>& aderite::audio::AudioController::getKnownEvents() const {
    return m_knownEvents;
}

void AudioController::unloadAll() {
    // Unload all then delete
    for (auto& instance : m_instances) {
        instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    }

    m_instances.clear();
}

} // namespace audio
} // namespace aderite
