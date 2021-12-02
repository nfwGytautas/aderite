#pragma once

#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace io {

using SerializableHandle = size_t;

/**
 * @brief Base class for serializable objects that require reference counting
 */
class SerializableAsset : public SerializableObject {
public:
    static constexpr size_t c_InvalidHandle = 0xffffffffffffffff; // std::numeric_limits<size_t>::max() or ULLONG_MAX

    virtual ~SerializableAsset();

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
     * @brief Releases a reference, this doesn't actually delete if a 0 is reached, this is controlled by the serializer
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
