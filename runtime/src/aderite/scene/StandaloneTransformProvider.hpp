#pragma once

#include "aderite/scene/TransformProvider.hpp"

namespace aderite {
namespace scene {

/**
 * @brief A transform provider that supplies values from itself
 */
class StandaloneTransformProvider final : public TransformProvider {
public:
    // Inherited via TransformProvider
    virtual const glm::vec3& getPosition() const override;
    virtual const glm::quat& getRotation() const override;
    virtual const glm::vec3& getScale() const override;
    virtual void setPosition(const glm::vec3& position) override;
    virtual void setRotation(const glm::quat& rotation) override;
    virtual void setScale(const glm::vec3& scale) override;

private:
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace scene
} // namespace aderite
