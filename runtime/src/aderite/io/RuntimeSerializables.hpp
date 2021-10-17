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

	// Last element of runtime serializables, used to specify the end runtime serializables
	RESERVED = 10,
	// END used to extend runtime serializables
	END
};

}
}
