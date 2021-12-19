#pragma once

namespace aderite {
namespace scripting {

/**
 * @brief Enum for getting the type of a field
 */
enum class FieldType {
    Null,

    // Value
    Float,
    Boolean,
    Integer,

    // Asset
    Mesh,
    Material,
    Audio,

    // Scene objects
    AudioSource,
};

} // namespace scripting
} // namespace aderite
