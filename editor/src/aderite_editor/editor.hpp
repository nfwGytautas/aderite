#pragma once

#include "aderite/interfaces/iaderite_editor.hpp"

namespace aderite {
	namespace editor {
		/**
		 * @brief Creates the editor depending on arguments, platform and other configs
		 * @param argc Number of console arguments
		 * @param argv Console arguments
		 * @return Editor instance, call delete when done
		*/
		interfaces::IEngineMiddleware* create_editor(int argc, char** argv);
	}
}