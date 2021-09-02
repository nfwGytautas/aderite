#pragma once

/**
 * @brief This file is used to define forward declarations for all rendering types
*/

#include "aderite/utility/Macros.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

class Renderer;
class DrawCall;

#if DEBUG_RENDER == 1
class DebugRenderer;
#endif

ADERITE_RENDERING_NAMESPACE_END
