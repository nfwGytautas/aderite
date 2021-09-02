#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Toolbar of the editor, this bar contains play, pause buttons etc.
*/
class Toolbar : public shared::BaseComponent {
public:
	Toolbar();
	virtual ~Toolbar();

	// Inherited via BaseComponent
	virtual void render() override;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
