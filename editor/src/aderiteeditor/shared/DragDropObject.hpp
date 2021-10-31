#pragma once

namespace aderite {
namespace editor {

/**
 * @brief Object passed to imgui, because imgui can't handle pointers correctly
 */
class DragDropObject {
public:
    void* Data;
};

} // namespace editor
} // namespace aderite
