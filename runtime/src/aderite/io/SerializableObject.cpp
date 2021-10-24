#include "SerializableObject.hpp"

#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

SerializableHandle SerializableObject::getHandle() const {
	return m_handle;
}

size_t SerializableObject::getRefCount() const {
	return m_refCount;
}

void SerializableObject::release() {
	ADERITE_DYNAMIC_ASSERT(m_refCount > 0, "Never acquired object was released");
	m_refCount--;
}

void SerializableObject::acquire() {
	m_refCount++;
}

}
}
