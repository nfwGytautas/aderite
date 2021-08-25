#pragma once

#include "aderite/interfaces/IEngineMiddleware.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN

/**
 * @brief Creates the editor depending on arguments, platform and other configs
 * @param argc Number of console arguments
 * @param argv Console arguments
 * @return Editor instance, call delete when done
*/
interfaces::IEngineMiddleware* createEditor(int argc, char** argv);

ADERITE_EDITOR_ROOT_NAMESPACE_END