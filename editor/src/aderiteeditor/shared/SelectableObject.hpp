#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Types for selectable types
*/
enum class SelectableObjectType {
	None,
	Asset,
	Entity,
};

/**
 * @brief Selectable object, that can be any of SelectableObjectType
*/
class SelectableObject {
public:
	SelectableObject();
	SelectableObject(io::SerializableObject* asset);
	SelectableObject(scene::Entity entity);

	/**
	 * @brief Returns the type of the object
	*/
	SelectableObjectType getType() const;

	/**
	 * @brief Returns the object part of the selectable
	*/
	io::SerializableObject* getAsset() const;

	/**
	 * @brief Returns the entity of the selectable
	*/
	scene::Entity getEntity() const;
private:
	SelectableObjectType m_type = SelectableObjectType::None;

	union {
		io::SerializableObject* Object;
		scene::Entity Entity;
	} m_data;
};

}
}
