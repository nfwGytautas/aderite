#pragma once

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Class that holds camera settings
 */
class CameraSettings : public io::ISerializable {
public:

private:
    float m_fov = 90.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;
};

} // namespace scene
} // namespace aderite
