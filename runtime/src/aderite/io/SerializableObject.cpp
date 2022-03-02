#include "SerializableObject.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace io {

SerializableObject::SerializableObject() : m_name(utility::generateString(12)) {}

std::string SerializableObject::getName() const {
    return m_name;
}

void SerializableObject::setName(const std::string& name) {
    m_name = name;
}

} // namespace io
} // namespace aderite
