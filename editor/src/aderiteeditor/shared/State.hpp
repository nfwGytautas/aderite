#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/SerializableObject.hpp"

#include "aderiteeditor/shared/Forward.hpp"

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

    /**
     * @brief True if currently in game mode, false otherwise
     */
    static bool IsGameMode;

    /**
     * @brief Returns the currently selected object
     */
    io::SerializableObject* getSelectedObject() const;

    /**
     * @brief Set the currently selected object
     * @param object The object to mark as selected
     */
    void setSelectedObject(io::SerializableObject* object);

    /**
     * @brief Returns the instance of the editor state
     */
    static State& getInstance();

private:
    io::SerializableObject* m_selectedObject = nullptr;
};

} // namespace editor
} // namespace aderite
