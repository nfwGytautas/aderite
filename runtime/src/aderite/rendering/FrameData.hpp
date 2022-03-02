#pragma once

#include <unordered_map>

#include <bgfx/bgfx.h>

#include "aderite/rendering/DrawCall.hpp"

namespace aderite {
namespace rendering {

struct CameraData {
    std::string Name;
    bgfx::TextureHandle Output;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

/**
 * @brief Object used to hold frame data
 */
struct FrameData {
    /**
     * @brief Draw calls
     */
    std::unordered_map<size_t, DrawCall> DrawCalls;

    /**
     * @brief Cameras
     */
    std::vector<CameraData> Cameras;
};

} // namespace rendering
} // namespace aderite
