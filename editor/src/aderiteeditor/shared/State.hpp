#pragma once

#include <bgfx/bgfx.h>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/runtime/Resolver.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

/**
 * @brief Global(makes life so much easier in this case) State of the editor
*/
class State {
public:
	static IEventSink* Sink;
	static Project* Project;
	static EditorCamera* EditorCamera;
	static bgfx::FrameBufferHandle DebugRenderHandle;
	static io::MappedResolver* Resolver;

	/**
	 * @brief True if currently in game mode, false otherwise
	*/
	static bool IsGameMode;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
