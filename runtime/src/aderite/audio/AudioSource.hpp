#pragma once

#include <fmod_studio.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/AudioSourceData.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio source object used to denote a point in the world where audio is emitted from
 */
class AudioSource final {
public:
    AudioSource(scene::GameObject* gObject);
    virtual ~AudioSource();

    /**
     * @brief Update the audio source properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Start playing audio
     */
    void start() const;

    /**
     * @brief Stop playing immediately
     */
    void stop() const;

    /**
     * @brief Returns the source data
     */
    AudioSourceData& getData();

private:
    scene::GameObject* m_gObject = nullptr;
    FMOD::Studio::EventInstance* m_instance = nullptr;
    AudioSourceData m_data;
};

} // namespace audio
} // namespace aderite
