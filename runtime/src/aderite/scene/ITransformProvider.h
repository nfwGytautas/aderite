#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace aderite {
namespace scene {

/**
 * @brief ITransformProvider is an interface for objects that have transform information
 */
class ITransformProvider {
public:
    virtual ~ITransformProvider() = default;

    virtual const glm::vec3& getPosition() const = 0;
    virtual const glm::quat& getRotation() const = 0;
    virtual const glm::vec3& getScale() const = 0;

    virtual void setPosition(const glm::vec3& position) = 0;
    virtual void setRotation(const glm::quat& rotation) = 0;
    virtual void setScale(const glm::vec3& scale) = 0;
};

} // namespace scene
} // namespace aderite
