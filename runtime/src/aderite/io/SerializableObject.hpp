#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "aderite/io/Forward.hpp"
#include "aderite/io/ISerializable.hpp"
#include "aderite/reflection/Reflectable.hpp"

namespace aderite {
namespace io {

/**
 * @brief A class that represents a single serializable object
 */
class SerializableObject : public ISerializable, public reflection::Reflectable {
public:
    SerializableObject();
    virtual ~SerializableObject() = default;

    /**
     * @brief Returns the name of the serializable object
     */
    std::string getName() const;

    /**
     * @brief Sets the name of the serializable object
     * @param name Name of the object
     */
    void setName(const std::string& name);

public:
    friend class Serializer; // Used to set the name
private:
    std::string m_name = "";
};

} // namespace io
} // namespace aderite
