#pragma once

/**
 * @brief This file contains all handle definitions in the engine
 */

#include <cstdint>

namespace aderite {
constexpr size_t c_InvalidHandle = 0xffffffffffffffff; // std::numeric_limits<size_t>::max() or ULLONG_MAX

namespace reflection {
using Type = size_t;
}

namespace io {
using SerializableHandle = size_t;
using LoadableHandle = size_t;
} // namespace io

namespace audio {
using AudioInstanceId = size_t;
}

namespace physics {
using ActorHandle = size_t;
}

} // namespace aderite
