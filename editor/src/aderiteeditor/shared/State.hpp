#pragma once

#include <bgfx/bgfx.h>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/shared/SelectableObject.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Global(makes life so much easier in this case) State of the editor
*/
class State {
public:
	static shared::IEventSink* Sink;
	static shared::Project* Project;
	static shared::EditorCamera* EditorCamera;
	static bgfx::FrameBufferHandle DebugRenderHandle;
	static SelectableObject LastSelectedObject;

	/**
	 * @brief True if currently in game mode, false otherwise
	*/
	static bool IsGameMode;
};

}
}
