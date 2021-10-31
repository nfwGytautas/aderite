#pragma once

#include <bgfx/bgfx.h>

#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/shared/SelectableObject.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Global(makes life so much easier in this case) State of the editor
 */
class State {
public:
    static editor::IEventSink* Sink;
    static editor::Project* Project;
    static editor::EditorCamera* EditorCamera;
    static bgfx::FrameBufferHandle DebugRenderHandle;
    static bgfx::FrameBufferHandle MeshPreviewHandle;
    static SelectableObject LastSelectedObject;

    /**
     * @brief True if currently in game mode, false otherwise
     */
    static bool IsGameMode;
};

} // namespace editor
} // namespace aderite
