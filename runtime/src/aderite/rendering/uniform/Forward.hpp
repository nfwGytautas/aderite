#pragma once

/**
 * @brief This file is used to define forward declarations for uniforms
*/

#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

// Base
class UniformManager;
class Uniform;

// Implementation
class SkyboxUniform;

#if DEBUG_RENDER == 1
class DebugUniform;
#endif

ADERITE_UNIFORM_NAMESPACE_END
