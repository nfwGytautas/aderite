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

    // Audio
    AUDIO_SOURCE = 20,
    AUDIO_LISTENER = 21,

    // Object
    VISUAL = 40,
    SCENERY = 41,
    ENTITY = 42,
    GAME_OBJECT = 43,

    // Other scene objects
    CAMERA = 60,

    // Geometry
    BOX_GEOMETRY = 75,

    // Physics
    PHYSICS_ACTOR = 90,
    DYNAMIC_ACTOR,
    STATIC_ACTOR,

    // Scripting
    SCRIPT_SYSTEM = 200,

    // Last element of runtime types, used to specify the end runtime serializables
    UNDEFINED = 249,
    RESERVED = 250,
    // END used to extend runtime types
    END
};

} // namespace reflection
} // namespace aderite
