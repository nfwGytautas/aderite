#include "State.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/rendering/Renderer.hpp"

#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

namespace aderite {
namespace editor {

editor::IEventSink* State::Sink = nullptr;
editor::Project* State::Project = nullptr;
bool State::IsGameMode = false;

io::SerializableObject* State::getSelectedObject() const {
    return m_selectedObject;
}

void State::setSelectedObject(io::SerializableObject* object) {
    m_selectedObject = object;
}

editor::EditorCamera* State::getEditorCamera() const {
    return m_editorCamera;
}

State& State::getInstance() {
    static State state;
    return state;
}

void State::init() {
    m_editorCamera = new editor::EditorCamera();
}

void State::shutdown() {
    delete m_editorCamera;
}

} // namespace editor
} // namespace aderite
