#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ScriptEventType.hpp"

#include "aderiteeditor/platform/pc/modals/IModal.hpp"

namespace aderite {
namespace editor {

/**
 * @brief This is the modal window showed on aderite editor when a script component is being added
 */
class SelectScriptModal : public IModal {
public:
    using SelectFn = std::function<void(scripting::ScriptEvent*)>;

public:
    SelectScriptModal(scripting::ScriptEventType filter, SelectFn fn);

    // Inherited via IModal
    void render() override;
    bool stillValid() const override;
    void close() override;
    void show() override;

private:
    bool m_open = true;
    scripting::ScriptEventType m_filtering = scripting::ScriptEventType::NONE;
    SelectFn m_callback;

    scripting::ScriptClass* m_class = nullptr;
    scripting::ScriptEvent* m_event = nullptr;
};

} // namespace editor
} // namespace aderite
