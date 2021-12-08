#pragma once

#include <vector>

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/rendering/Renderable.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Draw call information
 */
class DrawCall final {
public:
    // Any instance of a renderable to be used
    Renderable* Renderable = nullptr;

    // Vector containing transformations, for instanced rendering
    std::vector<glm::mat4> Transformations;
};

} // namespace rendering
} // namespace aderite
