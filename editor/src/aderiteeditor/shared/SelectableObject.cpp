#include "SelectableObject.hpp"

namespace aderite {
namespace editor {

SelectableObject::SelectableObject() 
	: m_type(SelectableObjectType::None), m_data{}
{}

aderite::editor::SelectableObject::SelectableObject(io::SerializableObject* asset)
	: m_type(SelectableObjectType::Asset), m_data{}
{
	m_data.Object = asset;
}

aderite::editor::SelectableObject::SelectableObject(scene::Entity entity) 
	: m_type(SelectableObjectType::Entity), m_data{}
{
	m_data.Entity = entity;
}

SelectableObjectType aderite::editor::SelectableObject::getType() const {
	return m_type;
}

io::SerializableObject* aderite::editor::SelectableObject::getAsset() const {
	return m_data.Object;
}

scene::Entity aderite::editor::SelectableObject::getEntity() const {
	return m_data.Entity;
}

}
}