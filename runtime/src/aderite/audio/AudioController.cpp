#include "AudioController.hpp"

#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <fmod_studio_common.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/audio/Bank.hpp"
#include "aderite/audio/AudioInstanceList.hpp"
#include "aderite/audio/AudioEvent.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

constexpr int c_MaxChannels = 512;

#ifdef _DEBUG
// Enable memory tracking for debug
constexpr FMOD_STUDIO_INITFLAGS c_StudioInitFlags = FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_MEMORY_TRACKING | FMOD_STUDIO_INIT_LIVEUPDATE;
constexpr FMOD_INITFLAGS c_InitFlags = FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE | FMOD_INIT_MEMORY_TRACKING;

//constexpr int c_MemoryEnable = FMOD_DEBUG_TYPE_MEMORY;
constexpr int c_MemoryEnable = 0;

FMOD_RESULT debugCallback(
	FMOD_DEBUG_FLAGS flags,
	const char* file,
	int line,
	const char* func,
	const char* message) 
{
	// Remove extra '\n'
	auto msg = std::string(message);
	msg.pop_back();

	if (flags & FMOD_DEBUG_LEVEL_ERROR) {
		LOG_ERROR("FMOD: {0} in function {1}", msg, func);
	} 
	else if (flags & FMOD_DEBUG_LEVEL_WARNING) {
		LOG_WARN("FMOD: {0} in function {1}", msg, func);
	}
	else if (flags & (FMOD_DEBUG_TYPE_TRACE | FMOD_DEBUG_LEVEL_LOG)) {
		LOG_TRACE("FMOD: {0}", msg);
	}
	else {
		if (flags & FMOD_DEBUG_TYPE_MEMORY) {
			LOG_TRACE("FMOD (memory): {0}", msg);
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
	if (FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG | FMOD_DEBUG_TYPE_TRACE | c_MemoryEnable, FMOD_DEBUG_MODE_CALLBACK, debugCallback, nullptr) != FMOD_OK) {
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
	// Configure listener
	bool thisFrameMute = false;
	int enabledListenerCount = 0;
	auto listenerGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
		.group<scene::components::AudioListenerComponent>(entt::get<scene::components::TransformComponent>);

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
			auto [listener, transform] = listenerGroup.get<
				scene::components::AudioListenerComponent,
				scene::components::TransformComponent>(entity);

			if (listener.IsEnabled) {

				FMOD_3D_ATTRIBUTES listener3dAttributes = {};

				listener3dAttributes.position = { transform.Position.x, transform.Position.y, transform.Position.z };

				// TODO: Rotation
				listener3dAttributes.up = { 0.0f, 1.0f, 0.0f };
				listener3dAttributes.forward = { 0.0f, 0.0f, 1.0f };

				// TODO: Velocity
				listener3dAttributes.velocity = { 0.0f, 0.0f, 0.0f };

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
	auto audioGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
		.group<scene::components::AudioSourcesComponent>(entt::get<scene::components::TransformComponent>);
	for (auto entity : audioGroup) {
		auto [audioSources, transform] = audioGroup.get<
			scene::components::AudioSourcesComponent,
			scene::components::TransformComponent>(entity);

		// Update instances

		for (AudioInstance* ai : *audioSources.Instances) {
			if (m_disabled) {
				// Stop
				if (ai->isInitialized()) {
					ai->stop();
					continue;
				}
			}

			// Init if needed
			if (ai->canBeInitialized()) {
				ai->init();
			}

			if (ai->isInitialized()) {
				if (m_wasDisabled && ai->getPlayOnStart()) {
					ai->start();
				}

				FMOD_3D_ATTRIBUTES source3dAttributes = {};

				// TODO: Volume
				if (m_mute || thisFrameMute) {
					if (ai->getFmodInstance()->setVolume(0.0f) != FMOD_OK) {
						LOG_WARN("Failed to set volume for audio instance");
					}
				}
				else {
					if (ai->getFmodInstance()->setVolume(1.0f) != FMOD_OK) {
						LOG_WARN("Failed to set volume for audio instance");
					}
				}

				source3dAttributes.position = { transform.Position.x, transform.Position.y, transform.Position.z };

				// TODO: Rotation
				source3dAttributes.up = { 0.0f, 1.0f, 0.0f };
				source3dAttributes.forward = { 0.0f, 0.0f, 1.0f };

				// TODO: Velocity
				source3dAttributes.velocity = { 0.0f, 0.0f, 0.0f };

				ai->getFmodInstance()->set3DAttributes(&source3dAttributes);
			}
		}
	}

	if (!m_disabled) {
		m_wasDisabled = false;
	}

	if (m_fmodSystem->update() != FMOD_OK) {
		LOG_WARN("Failed to update audio controller");
	}
}

void AudioController::loadMasterBank(const std::filesystem::path& dir) {
	// Setup audio controller
	std::filesystem::path resDir = ::aderite::Engine::getAssetManager()->getResDir();

	LOG_DEBUG("Loading master banks from {0}", (resDir / dir).string());

	// Check if banks exist
	std::filesystem::path masterPath = (resDir / dir / "Master.bank");
	std::filesystem::path stringPath = (resDir / dir / "Master.strings.bank");

	if (!std::filesystem::exists(masterPath) || !std::filesystem::exists(stringPath)) {
		LOG_WARN("The directory didn't contain 'Master.bank' and 'Master.strings.bank' file, leaving unchanged");
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
	if (m_fmodSystem->loadBankFile(stringPath.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_stringBank) != FMOD_OK) {
		LOG_WARN("Failed to load strings bank from {0}", stringPath.string());
		return;
	}

	if (m_fmodSystem->loadBankFile(masterPath.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank) != FMOD_OK) {
		LOG_WARN("Failed to load master bank from {0}", masterPath.string());
		m_stringBank->unload();
		return;
	}
}

bool AudioController::masterBanksLoaded() const {
	return m_masterBank != nullptr && m_stringBank != nullptr;
}

bool AudioController::has(const std::string& name) {
	auto it = std::find_if(m_banks.begin(), m_banks.end(), [&](Bank* bank) {
		return bank->getName() == name;
	});

	return it != m_banks.end();
}

Bank* AudioController::readBank(const std::string& name) {
	// Check for duplication
	if (has(name)) {
		LOG_WARN("Requested double read");
		return getBank(name);
	}

	LOG_TRACE("Audio controller reading bank {0}", name);

	Bank* bank = new Bank(m_fmodSystem, name);
	m_banks.push_back(bank);
	return bank;
}

Bank* AudioController::getOrRead(const std::string& name) {
	if (!has(name)) {
		return readBank(name);
	}
	else {
		return getBank(name);
	}
}

Bank* AudioController::getBank(const std::string& name) {
	auto it = std::find_if(m_banks.begin(), m_banks.end(), [&](Bank* bank) {
		return bank->getName() == name;
	});

	if (it == m_banks.end()) {
		LOG_WARN("Tried to get non existing bank with name {0}", name);
		return nullptr;
	}

	return *it;
}

void AudioController::unloadBank(const std::string& name) {
	auto it = std::find_if(m_banks.begin(), m_banks.end(), [&](Bank* bank) {
		return bank->getName() == name;
	});

	if (it == m_banks.end()) {
		return;
	}

	delete (*it);
	m_banks.erase(it);
}

void AudioController::unloadBank(Bank* bank) {
	unloadBank(bank->getName());
}

void AudioController::setMute(bool value) {
	m_mute = value;
}

void AudioController::disable(bool value) {
	m_wasDisabled = m_disabled;
	m_disabled = value;
}

void AudioController::unloadAll() {
	// Unload all then delete
	for (auto& bank : m_banks) {
		bank->unloadData();
	}

	for (auto& bank : m_banks) {
		delete bank;
	}

	m_banks.clear();
}

ADERITE_AUDIO_NAMESPACE_END
