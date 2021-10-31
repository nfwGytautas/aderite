#include "editor.hpp"

// TODO: Macro switch
#include "aderiteeditor/windows/WindowsEditor.hpp"

namespace aderite {

interfaces::IEngineMiddleware* createEditor(int argc, char** argv) {
    // TODO: Macro switch
#if defined(WIN32) || defined(_WIN32)
    return new WindowsEditor(argc, argv);
#endif

    return nullptr;
}

} // namespace aderite
