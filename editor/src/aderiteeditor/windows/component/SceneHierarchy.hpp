#pragma once

#include "aderite/scene/Entity.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Scene view is a component that displays all scene entities in a tree
*/
class SceneHierarchy : public shared::BaseComponent {
public:
	SceneHierarchy();
	virtual ~SceneHierarchy();

	// Inherited via BaseComponent
	virtual void render() override;

	/**
	 * @brief Sets the current active Entity (used to identify which is active)
	*/
	void setActiveEntity(scene::Entity& Entity);
private:
	scene::Entity m_selectedEntity = scene::Entity::null();
	TextInputModal* m_textModal = nullptr;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
