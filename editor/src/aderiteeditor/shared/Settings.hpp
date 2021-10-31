#pragma once

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Various settings for editor
 */
class Settings {
public:
    /**
     * @brief Pan speed of the editor camera, default 0.01
     */
    static float EditorCameraPanSpeed;

    /**
     * @brief Zoom speed of the editor camera, default 0.6
     */
    static float EditorCameraZoomSpeed;

    /**
     * @brief Rotation speed of the editor camera, default 0.8
     */
    static float EditorCameraRotationSpeed;

    /**
     * @brief Field of vision of the editor camera, default 90.0f
     */
    static float EditorCameraFov;
};

} // namespace editor
} // namespace aderite
