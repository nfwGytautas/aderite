#pragma once

/**
 * @brief This file contains all serializable types that the runtime has
 */

#include "aderite/reflection/Forward.hpp"

namespace aderite {
namespace reflection {

/**
 * @brief Enum containing all types that the runtime has
 */
enum class RuntimeTypes : Type {
    // Assets
    MESH = 0,
    MATERIAL = 1,
    TEXTURE = 2,
    SCENE = 3,
    MAT_TYPE = 4,
    PIPELINE = 5,
    AUDIO = 6,
    PREFAB = 7,

    // Object
    GAME_OBJECT = 40,

    // Geometry
    BOX_GEOMETRY = 75,

    // Last element of runtime types, used to specify the end runtime serializables
    UNDEFINED = 249,
    RESERVED = 250,
    // END used to extend runtime types
    END
};

} // namespace reflection
} // namespace aderite
