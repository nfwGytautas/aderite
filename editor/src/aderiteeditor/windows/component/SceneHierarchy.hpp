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
private:
	TextInputModal* m_textModal = nullptr;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
