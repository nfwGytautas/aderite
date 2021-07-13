#include <iostream>

#include "aderite/aderite.hpp"
#include "aderite_editor/editor.hpp"

int main(int argc, char** argv) {
	// Create and attach editor
	aderite::interfaces::iaderite_editor* editor = aderite::editor::create_editor(argc, argv);
	aderite::engine::get()->attach_editor(editor);

	if (aderite::engine::get()->init({})) {
		// Initialized successfully
		aderite::engine::get()->loop();
		aderite::engine::get()->shutdown();
	}

	return 0;
}