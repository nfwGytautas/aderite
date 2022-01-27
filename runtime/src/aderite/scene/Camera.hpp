#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Camera class used as a main way to see rendering results
 */
class Camera : public io::ISerializable {
public:
    Camera();
    ~Camera();

    /**
     * @brief Get field of vision of the camera
     */
    float getFoV() const;

    /**
     * @brief Set field of vision of the camera
     * @param fov New fov value
     */
    void setFoV(float fov);

    /**
     * @brief Returns the texture output of the Camera
     */
    bgfx::TextureHandle getOutputHandle() const;

    // Inherited via SerializableObject
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    bgfx::TextureHandle m_output = BGFX_INVALID_HANDLE;

    float m_fov = 90.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;
};

} // namespace scene
} // namespace aderite
