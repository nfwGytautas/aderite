#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/scene/CameraSettings.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Camera class used as a main way to see rendering results
 */
class Camera final {
public:
    Camera(scene::GameObject* gObject);
    ~Camera();

    /**
     * @brief Update the camera properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Returns the texture output of the Camera
     */
    bgfx::TextureHandle getOutputHandle() const;

    /**
     * @brief Returns the camera data
     */
    CameraSettings& getData();

    /**
     * @brief Returns the forward vector of this camera, can be used for raycasting from the camera point of view
     */
    glm::vec3 getForwardDirection() const;

private:
    scene::GameObject* m_gObject = nullptr;
    bgfx::TextureHandle m_output = BGFX_INVALID_HANDLE;

    CameraSettings m_settings;
};

} // namespace scene
} // namespace aderite
