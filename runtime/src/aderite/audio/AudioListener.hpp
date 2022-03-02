#pragma once

#include <string>

#include "aderite/audio/AudioListenerData.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio listener object used to denote a point in the world where audio is heard from
 */
class AudioListener final {
public:
    AudioListener(scene::GameObject* gObject);
    virtual ~AudioListener();

    /**
     * @brief Update the audio listener properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Returns the audio listener data
     */
    AudioListenerData& getData();

private:
    scene::GameObject* m_gObject = nullptr;
    AudioListenerData m_data;
};

} // namespace audio
} // namespace aderite
