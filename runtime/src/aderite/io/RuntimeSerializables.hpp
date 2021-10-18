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
	MESH		= 0,
	MATERIAL	= 1,
	TEXTURE		= 2,
	SCENE		= 3,
	MAT_TYPE	= 4,

	// Last element of runtime serializables, used to specify the end runtime serializables
	RESERVED = 10,
	// END used to extend runtime serializables
	END
};

}
}
