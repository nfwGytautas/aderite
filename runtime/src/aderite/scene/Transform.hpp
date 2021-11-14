#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace scene {

/**
 * @brief A transform for describing a position in the scene/world
 */
class Transform final : public io::ISerializable {
public:
    /**
     * @brief Returns a mutable position vector
     */
    glm::vec3& position() {
        return m_position;
    }

    /**
     * @brief Returns a immutable position vector
     */
    const glm::vec3& position() const {
        return m_position;
    }

    /**
     * @brief Returns a mutable rotation vector
     */
    glm::quat& rotation() {
        return m_rotation;
    }

    /**
     * @brief Returns a immutable rotation vector
     */
    const glm::quat& rotation() const {
        return m_rotation;
    }

    /**
     * @brief Returns a mutable scale vector
     */
    glm::vec3& scale() {
        return m_scale;
    }

    /**
     * @brief Returns a immutable scale vector
     */
    const glm::vec3& scale() const {
        return m_scale;
    }

    /**
     * @brief Computes the transformation matrix of the component
     * @param t Transform component
     * @return Computed transformation matrix
     */
    glm::mat4 computeTransform() const;

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace scene
} // namespace aderite