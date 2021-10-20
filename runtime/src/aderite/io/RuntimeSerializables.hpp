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
	// Assets
	MESH		= 0,
	MATERIAL	= 1,
	TEXTURE		= 2,
	SCENE		= 3,
	MAT_TYPE	= 4,
	PIPELINE	= 5,

	// Colliders
	CLDR_LIST	= 10,
	BOX_CLDR	= 11,

	// Last element of runtime serializables, used to specify the end runtime serializables
	RESERVED = 30,
	// END used to extend runtime serializables
	END
};

}
}
