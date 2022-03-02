#pragma once

#include <vector>

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/asset/Forward.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Draw call information
 */
class DrawCall final {
public:
    // Mesh of draw call
    asset::MeshAsset* Mesh = nullptr;

    // Material of draw call
    asset::MaterialAsset* Material = nullptr;

    // Vector containing transformations, for instanced rendering
    std::vector<glm::mat4> Transformations;
};

} // namespace rendering
} // namespace aderite
