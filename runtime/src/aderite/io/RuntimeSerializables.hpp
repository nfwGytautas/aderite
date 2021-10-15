#pragma once

/**
 * @brief This file contains all serializable types that the runtime has
*/

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Enum containing all serializable types that the runtime has
*/
enum class RuntimeSerializables : SerializableType {
	SCENE,
	MESH,
	MATERIAL,
	TEXTURE,

	// END used to extend runtime serializables
	END
};

}
}
