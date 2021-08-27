#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/macros.hpp"

ADERITE_UTILITY_NAMESPACE_BEGIN

/**
 * @brief Utility function used to create a framebuffer, caller is responsible for destroying it
 * @param hdr If true the framebuffer will be created with HDR color format
 * @param depth If true the framebuffer will include depth buffer
 * @return bgfx framebuffer handle
*/
bgfx::FrameBufferHandle createFrameBuffer(bool hdr = true, bool depth = true);

ADERITE_UTILITY_NAMESPACE_END
