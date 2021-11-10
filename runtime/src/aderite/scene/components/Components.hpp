#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
 */

namespace aderite {
namespace scene {

/**
 * @brief Camera component, which references ICamera object
 */
struct CameraComponent {
    bool Main = true;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

} // namespace scene
} // namespace aderite
