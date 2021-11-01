#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace aderite {
namespace scene {

/**
 * @brief Transform component, used to situate an Entity in world space
 */
struct TransformComponent {
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::quat Rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) :
        Position(position),
        Rotation(rotation),
        Scale(scale) {}

    /**
     * @brief Computes the transformation matrix of the component
     * @param t Transform component
     * @return Computed transformation matrix
     */
    static glm::mat4 computeTransform(const TransformComponent& t) {
        glm::mat4 rotation = glm::toMat4(t.Rotation);
        return glm::translate(glm::mat4(1.0f), t.Position) * rotation * glm::scale(glm::mat4(1.0f), t.Scale);
    }
};

} // namespace scene
} // namespace aderite
