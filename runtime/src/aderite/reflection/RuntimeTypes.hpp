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

    // Colliders
    CLDR_LIST = 10,
    BOX_CLDR = 11,

    // Operations
    OP_CAMERA = 20,
    OP_ENTITY = 21,
    OP_SCREEN = 22,
    OP_RENDER = 23,
    OP_TARGET = 24,

    // Scripting
    SCRIPT_LIST = 40,
    SCRIPT = 41,

    // Last element of runtime types, used to specify the end runtime serializables
    RESERVED = 50,
    // END used to extend runtime types
    END
};

} // namespace reflection
} // namespace aderite
