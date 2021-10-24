#pragma once

/**
 * @brief This file contains all handle definitions in the engine
*/


namespace aderite {
constexpr size_t c_InvalidHandle = 0xffffffffffffffffui64; // std::numeric_limits<size_t>::max() or ULLONG_MAX

namespace reflection {
using Type = size_t;
}

namespace io {
using SerializableHandle = size_t;
using LoadableHandle = size_t;
}

namespace audio {
using AudioInstanceId = size_t;
}

}