#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Camera class used as a main way to see rendering results
 */
class Camera : public io::SerializableObject {
public:
    Camera();
    ~Camera();

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);

    const glm::vec3& getRotation() const;
    void setRotation(const glm::vec3& rotation);

    /**
     * @brief Returns the view matrix of the editor camera
     */
    const glm::mat4& getViewMatrix() const;

    /**
     * @brief Returns the projection matrix of the editor camera
     */
    const glm::mat4& getProjectionMatrix() const;

    /**
     * @brief Returns the texture output of the Camera
     */
    bgfx::TextureHandle getOutputHandle() const;

    // Inherited via SerializableObject
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    virtual reflection::Type getType() const override;

private:
    void updateMatrices();

private:
    bgfx::TextureHandle m_output = BGFX_INVALID_HANDLE;

    glm::vec3 m_position = {};

    // Euler angles encoded in vec3 (pitch, yaw, roll)
    glm::vec3 m_eulerRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    float m_fov = 90.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;

    glm::mat4 m_viewMatrix = {};
    glm::mat4 m_projectionMatrix = {};
};

} // namespace scene
} // namespace aderite
