#pragma once

#include <string>
#include <type_traits>

#include "aderite/reflection/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace reflection {

/**
 * @brief Class used to map a serialized type identifier to serializable object instance.
 * Paired with type map in the Serializer this gives a minimal level of reflection.
 */
class InstancerBase {
public:
    virtual ~InstancerBase() {}

    /**
     * @brief Create Reflectable instance
     * @return Reflectable instance
     */
    virtual Reflectable* create() = 0;
};

/**
 * @brief Templated Instancer used to create object instance
 * @tparam T Instancer type, T must inherit from Reflectable
 */
template<typename T>
class Instancer : public InstancerBase {
public:
    virtual Reflectable* create() override {
        ADERITE_STATIC_ASSERT((std::is_base_of<Reflectable, T>::value), "Instancer can only be used with types that inherit from "
                                                                        "Reflectable");
        return static_cast<Reflectable*>(new T());
    }
};

} // namespace reflection
} // namespace aderite
