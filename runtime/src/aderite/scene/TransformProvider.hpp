#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief TransformProvider is an interface for objects that have transform information
 */
class TransformProvider final : public io::ISerializable {
public:
    virtual ~TransformProvider() = default;

    /**
     * @brief Returns true if the transform was modified, false otherwise
     */
    bool wasModified() const;

    /**
     * @brief Resets the modified flag
     */
    void resetModifiedFlag();

    /**
     * @brief Returns the position value
     */
    const glm::vec3& getPosition() const;

    /**
     * @brief Returns the rotation value
     */
    const glm::quat& getRotation() const;

    /**
     * @brief Returns the scale value
     */
    const glm::vec3& getScale() const;

    /**
     * @brief Sets the position
     * @param position New position value
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Sets the rotation
     * @param rotation New rotation value
     */
    void setRotation(const glm::quat& rotation);

    /**
     * @brief Sets the scale
     * @param scale New scale value
     */
    void setScale(const glm::vec3& scale);

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    TransformProvider& operator=(const TransformProvider& other);

private:
    bool m_wasModified = false;
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace scene
} // namespace aderite
