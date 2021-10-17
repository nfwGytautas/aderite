#pragma once

#include "aderite/utility/Macros.hpp"
#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Class used to map a serialized type identifier to serializable object instance.
 * Paired with type map in the Serializer this gives a minimal level of reflection.
*/
class InstancerBase {
public:
	virtual ~InstancerBase() {}

	/**
	 * @brief Create SerializableObject instance
	 * @return SerializableObject instance
	*/
	virtual SerializableObject* create() = 0;
};

/**
 * @brief Templated Instancer used to create object instance
 * @tparam T Instancer type, T must inherit from SerializableObject
*/
template<typename T>
class Instancer : public InstancerBase {
public:
	virtual SerializableObject* create() override {
		ADERITE_STATIC_ASSERT(std::is_base_of<SerializableObject, T>::value, "Instancer can only be used with types that inherit from SerializableObject");
		return static_cast<SerializableObject*>(new T());
	}
};

}
}
