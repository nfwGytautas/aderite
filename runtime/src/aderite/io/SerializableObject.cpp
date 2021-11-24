#include "SerializableObject.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

SerializableObject::~SerializableObject() {
    if (m_refCount > 0) {
        LOG_ERROR("[Asset] Destroying {0}, but it still has {1} outstanding references", m_refCount);
        ADERITE_DYNAMIC_ASSERT(m_refCount == 0, "Asset being destroyed with outstanding references");
    } else {
        LOG_TRACE("[Asset] Destroying {0}, it has no outstanding references", m_handle);
    }
}

SerializableHandle SerializableObject::getHandle() const {
    return m_handle;
}

size_t SerializableObject::getRefCount() const {
    return m_refCount;
}

void SerializableObject::release() {
    LOG_TRACE("[Asset] Releasing {0} reference, there are currently {1} references", m_handle, m_refCount);
    ADERITE_DYNAMIC_ASSERT(m_refCount > 0, "Never acquired object was released");
    m_refCount--;
}

void SerializableObject::acquire() {
    LOG_TRACE("[Asset] Acquiring {0} reference, there are currently {1} references", m_handle, m_refCount);
    m_refCount++;
}

} // namespace io
} // namespace aderite
