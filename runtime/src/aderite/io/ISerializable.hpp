#pragma once

#include <yaml-cpp/yaml.h>

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief ISerializable provides an interface for serializing various objects
 */
class ISerializable {
public:
    virtual ~ISerializable() = default;

    /**
     * @brief Serialize object to emmitter
     * @param serializer Serializer instance
     * @param emitter Emmitter to serialize to
     * @return True if serialized without errors, false otherwise
     */
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const = 0;

    /**
     * @brief Deserialize object from data node
     * @param serializer Serializer instance
     * @param data Data node
     * @return True if deserialized, false otherwise
     */
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) = 0;
};

} // namespace io
} // namespace aderite
