#include "SelectableObject.hpp"

namespace aderite {
namespace editor {

SelectableObject::SelectableObject() : m_type(SelectableObjectType::None), m_data {} {}

aderite::editor::SelectableObject::SelectableObject(io::SerializableObject* object) : m_type(SelectableObjectType::Object), m_data {} {
    m_data.Object = object;
}

SelectableObjectType aderite::editor::SelectableObject::getType() const {
    return m_type;
}

io::SerializableObject* SelectableObject::getSerializable() const {
    return m_data.Object;
}

} // namespace editor
} // namespace aderite