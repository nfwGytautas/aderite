#pragma once

#include <bgfx/bgfx.h>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Viewport of the editor, this is the window where the game is actually rendered to
*/
class Viewport : public shared::BaseComponent {
public:
	// Inherited via BaseComponent
	virtual void init() override;
	virtual void shutdown() override;
	virtual void render() override;

private:
	bgfx::FrameBufferHandle m_fbh;
	bgfx::TextureHandle m_fbth;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
