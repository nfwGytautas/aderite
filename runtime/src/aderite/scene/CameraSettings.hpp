#pragma once

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Class that holds camera settings
 */
class CameraSettings : public io::ISerializable {
public:
    virtual ~CameraSettings();

    /**
     * @brief Get field of vision
     */
    float getFoV() const;

    /**
     * @brief Set field of vision
     * @param fov New fov value
     */
    void setFoV(float fov);

    /**
     * @brief Get near clipping plane distance
     */
    float getNearClip() const;

    /**
     * @brief Set the near clipping plane distance
     * @param distance
     */
    void setNearClip(float distance);

    /**
     * @brief Get far clipping plane distance
     */
    float getFarClip() const;

    /**
     * @brief Set the far clipping plane distance
     * @param distance
     */
    void setFarClip(float distance);

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    CameraSettings& operator=(const CameraSettings& other);

private:
    float m_fov = 90.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;
};

} // namespace scene
} // namespace aderite
