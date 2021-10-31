#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "aderite/io/Forward.hpp"
#include "aderite/reflection/Reflectable.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

/**
 * @brief Base class providing serializable interface
 */
class ISerializable : public reflection::Reflectable {
public:
    virtual ~ISerializable() {}

    /**
     * @brief Serialize object to emmitter
     * @param serializer Serializer instance
     * @param emitter Emmitter to serialize to
     * @return True if serialized without errors, false otherwise
     */
    virtual bool serialize(const Serializer* serializer, YAML::Emitter& emitter) = 0;

    /**
     * @brief Deserialize object from data node
     * @param serializer Serializer instance
     * @param data Data node
     * @return True if deserialized, false otherwise
     */
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) = 0;
};

/**
 * @brief Base class for all serializable objects (object is something that has an ID)
 */
class SerializableObject : public ISerializable {
public:
    virtual ~SerializableObject() {}

    /**
     * @brief Returns the handle of this serializable object
     * @return Object handle
     */
    SerializableHandle getHandle() const;

    /**
     * @brief Returns the number of outstanding references to this object
     */
    size_t getRefCount() const;

    /**
     * @brief Releases a reference, this doesn't actually delete if a 0 is reached, this is mandated by the serializer
     */
    void release();

    /**
     * @brief Acquires a reference
     */
    void acquire();

public:
    friend class Serializer; // Used to set the handle
private:
    SerializableHandle m_handle = c_InvalidHandle;
    size_t m_refCount = 0;
};

} // namespace io
} // namespace aderite
