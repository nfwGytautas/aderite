#pragma once

#include "aderite/interfaces/IEngineMiddleware.hpp"

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {

/**
 * @brief Creates the editor depending on arguments, platform and other configs
 * @param argc Number of console arguments
 * @param argv Console arguments
 * @return Editor instance, call delete when done
 */
interfaces::IEngineMiddleware* createEditor(int argc, char** argv);

} // namespace aderite