#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"


ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief NodeEditor of the editor, this is used by FPRP
*/
class NodeEditor : public shared::BaseComponent {
public:
	NodeEditor();
	virtual ~NodeEditor();

	// Inherited via BaseComponent
	virtual void init() override;
	virtual void render() override;
	virtual void shutdown() override;

private:
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
