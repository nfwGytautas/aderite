#pragma once

#include <vector>

#include "aderite/audio/Forward.hpp"

namespace aderite {
namespace audio {

/**
 * @brief AudioRequest is a collection of one shot audio and instances running on a source
 */
struct AudioRequest {
    std::vector<AudioInstance*> Oneshot;
    std::vector<AudioInstance*> Instances;
};

} // namespace audio
} // namespace aderite
