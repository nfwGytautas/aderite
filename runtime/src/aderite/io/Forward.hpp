#pragma once

/**
 * @brief This file is used to define forward declarations for all 
 * serialization related objects
*/

namespace aderite {
namespace io {

// Typedefs
using SerializableHandle = size_t;
using SerializableType = size_t;
constexpr size_t c_InvalidHandle = 0xffffffffffffffffui64; // std::numeric_limits<size_t>::max() or ULLONG_MAX

// Classes
class Serializer;
class InstancerBase;
class SerializableObject;
class PathResolver;
struct Path;

}
}
