#include <iostream>

#include "aderite/Aderite.hpp"

#include "aderiteeditor/Editor.hpp"

int main(int argc, char** argv) {
    // Create and attach editor
    aderite::interfaces::IEngineMiddleware* editor = aderite::createEditor(argc, argv);
    ::aderite::Engine::get()->attachMiddleware(editor);

    if (::aderite::Engine::get()->init({})) {
        // Initialized successfully
        ::aderite::Engine::get()->loop();
        ::aderite::Engine::get()->shutdown();
    }

    return 0;
}