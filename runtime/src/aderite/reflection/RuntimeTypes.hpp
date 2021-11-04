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

    // Colliders
    CLDR_LIST = 50,
    BOX_CLDR,

    // Physics actors
    PHYSICS_ACTOR = 90,
    DYNAMIC_ACTOR,
    STATIC_ACTOR,

    // Operations
    OP_CAMERA = 100,
    OP_ENTITY,
    OP_SCREEN,
    OP_RENDER,
    OP_TARGET,

    // Scripting
    SCRIPT_LIST = 200,
    SCRIPT,

    // Last element of runtime types, used to specify the end runtime serializables
    RESERVED,
    // END used to extend runtime types
    END
};

} // namespace reflection
} // namespace aderite
