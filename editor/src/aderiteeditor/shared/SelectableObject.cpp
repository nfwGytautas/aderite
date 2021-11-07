#include "SelectableObject.hpp"

namespace aderite {
namespace editor {

SelectableObject::SelectableObject() : m_type(SelectableObjectType::None), m_data {} {}

aderite::editor::SelectableObject::SelectableObject(io::SerializableObject* asset) : m_type(SelectableObjectType::Asset), m_data {} {
    m_data.Object = asset;
}

aderite::editor::SelectableObject::SelectableObject(scene::EditorEntity* entity) : m_type(SelectableObjectType::Entity), m_data {} {
    m_data.Entity = entity;
}

SelectableObject::SelectableObject(io::ISerializable* serializable) : m_type(SelectableObjectType::Serializable), m_data {} {
    m_data.Serializable = serializable;
}

SelectableObjectType aderite::editor::SelectableObject::getType() const {
    return m_type;
}

io::SerializableObject* aderite::editor::SelectableObject::getAsset() const {
    return m_data.Object;
}

scene::EditorEntity* aderite::editor::SelectableObject::getEntity() const {
    return m_data.Entity;
}

io::ISerializable* SelectableObject::getSerializable() const {
    return m_data.Serializable;
}

} // namespace editor
} // namespace aderite