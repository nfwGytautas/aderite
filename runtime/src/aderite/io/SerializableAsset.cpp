#include "SerializableAsset.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

SerializableAsset::~SerializableAsset() {
    if (m_refCount > 0) {
        LOG_ERROR("[Asset] Destroying {0}, but it still has {1} outstanding references", m_refCount);
        ADERITE_DYNAMIC_ASSERT(m_refCount == 0, "Asset being destroyed with outstanding references");
    } else {
        LOG_TRACE("[Asset] Destroying {0}, it has no outstanding references", m_handle);
    }
}

SerializableHandle SerializableAsset::getHandle() const {
    return m_handle;
}

size_t SerializableAsset::getRefCount() const {
    return m_refCount;
}

void SerializableAsset::release() {
    LOG_TRACE("[Asset] Releasing {0}(handle: {2}) reference, there are currently {1} references", this->getName(), m_refCount, m_handle);
    ADERITE_DYNAMIC_ASSERT(m_refCount > 0, "Never acquired object was released");
    m_refCount--;
}

void SerializableAsset::acquire() {
    LOG_TRACE("[Asset] Acquiring {0}(handle: {2}) reference, there are currently {1} references", this->getName(), m_refCount, m_handle);
    m_refCount++;
}

void SerializableAsset::load(const io::Loader* loader) {}

void SerializableAsset::unload() {}

bool SerializableAsset::needsLoading() const {
    return false;
}

} // namespace io
} // namespace aderite
