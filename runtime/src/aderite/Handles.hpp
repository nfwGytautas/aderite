#pragma once

/**
 * @brief This file contains all handle definitions in the engine
 */

#include <cstdint>

namespace aderite {
constexpr size_t c_InvalidHandle = 0xffffffffffffffff; // std::numeric_limits<size_t>::max() or ULLONG_MAX

using HandleType = size_t;
using UniversalHandle = HandleType;

namespace reflection {
using Type = HandleType;
}

namespace io {
using SerializableHandle = HandleType;
using LoadableHandle = HandleType;
} // namespace io

namespace audio {
using SourceHandle = HandleType;
}

namespace scene {
using TransformHandle = HandleType;
}

} // namespace aderite
