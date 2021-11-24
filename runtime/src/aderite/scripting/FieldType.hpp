#pragma once

namespace aderite {
namespace scripting {

/**
 * @brief Enum for getting the type of a field
 */
enum class FieldType {
    Null,
    Float,
    Boolean,
    Object,
    Mesh,
    Material,
    System,
    Audio,
    AudioSource,
    Prefab,
};

} // namespace scripting
} // namespace aderite
