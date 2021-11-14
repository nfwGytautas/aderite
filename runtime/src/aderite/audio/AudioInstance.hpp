#pragma once

#include <string>

#include <fmod_studio.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace aderite {
namespace audio {

/**
 * @brief Audio instance class that wraps around FMOD primitives to provide an audio API
 */
class AudioInstance final {
public:
    ~AudioInstance();

    /**
     * @brief Start playing audio
     */
    void start();

    /**
     * @brief Stop playing immediately
     */
    void stop();

    /**
     * @brief Sets the volume of the instance
     * @param volume Volume from 0.0 to 1.0
     */
    void setVolume(float volume);

    /**
     * @brief Sets the spatial attributes of the audio instance
     * @param position Position in the world for audio
     * @param rotation Rotation of the audio source
     * @param velocity Velocity at which the audio is moving
     */
    void setAttributes(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& velocity);

private:
    AudioInstance(FMOD::Studio::EventInstance* instance);
    friend class AudioController;

private:
    std::string m_name = "";
    FMOD::Studio::EventInstance* m_instance = nullptr;
};

} // namespace audio
} // namespace aderite
