#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Entity editor is a component that allows to edit specific selected Entity information
*/
class EntityEditor : public shared::BaseComponent {
public:
	EntityEditor();
	virtual ~EntityEditor();

	// Inherited via BaseComponent
	virtual void render() override;

	/**
	 * @brief Sets the current active Entity
	*/
	void setActiveEntity(scene::Entity& Entity);
private:
	scene::Entity m_selectedEntity = scene::Entity::null();
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
