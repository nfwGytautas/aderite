#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/AudioRequest.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Component containing information about audio source on an entity (1 per entity)
 */
struct AudioSourceComponent {
    audio::AudioRequest Request;

    // Properties
    float Volume = 1.0f;

    AudioSourceComponent() = default;
    AudioSourceComponent(const AudioSourceComponent&) = default;
};

/**
 * @brief Component used to specify that this entity is listening to audio inputs
 */
struct AudioListenerComponent {
    bool IsEnabled = false;

    AudioListenerComponent() = default;
    AudioListenerComponent(const AudioListenerComponent&) = default;
};

} // namespace scene
} // namespace aderite
