#pragma once

#include "aderite/asset/Forward.hpp"

namespace aderite {
namespace scene {

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
