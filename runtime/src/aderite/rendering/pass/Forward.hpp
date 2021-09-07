#pragma once

/**
 * @brief This file is used to define forward declarations for all pass types
*/

#include "aderite/utility/Macros.hpp"

ADERITE_PASS_NAMESPACE_BEGIN

class IblPass;
class SkyPass;

#if DEBUG_RENDER == 1
class DebugPass;
#endif

ADERITE_PASS_NAMESPACE_END
