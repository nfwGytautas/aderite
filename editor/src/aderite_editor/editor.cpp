#include "editor.hpp"

// TODO: Macro switch
#include "aderite_editor/windows/windows_editor.hpp"

namespace aderite {
	namespace editor {
		interfaces::IEngineMiddleware* create_editor(int argc, char** argv) {
#if defined(WIN32) || defined(_WIN32)
			return new windows_editor(argc, argv);
#endif

			return nullptr;
		}
	}
}
