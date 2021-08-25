#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Toolbar of the editor, the bar that goes at the top of the window
*/
class Toolbar : public shared::BaseComponent {
public:
	Toolbar();
	virtual ~Toolbar();

	// Inherited via BaseComponent
	virtual void render() override;
private:
	TextInputModal* m_textModal = nullptr;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
