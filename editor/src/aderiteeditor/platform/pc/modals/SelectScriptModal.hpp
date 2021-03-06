#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "aderite/scripting/Forward.hpp"

#include "aderiteeditor/platform/pc/modals/IModal.hpp"

namespace aderite {
namespace editor {

/**
 * @brief This is the modal window showed on aderite editor when a script component is being added
 */
class SelectScriptModal : public IModal {
public:
    using SelectFn = std::function<void(scripting::BehaviorBase*)>;

public:
    SelectScriptModal(SelectFn fn);

    // Inherited via IModal
    void render() override;
    bool stillValid() const override;
    void close() override;
    void show() override;

private:
    bool m_open = true;
    SelectFn m_callback;
};

} // namespace editor
} // namespace aderite
