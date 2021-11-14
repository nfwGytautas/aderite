#pragma once

#include "aderite/scene/Forward.hpp"

namespace aderite {
class Engine;

namespace scene {
/**
 * @brief Scene manager for aderite
 */
class SceneManager final {
public:
    // TODO: Queue scene load API

    /**
     * @brief Initializes the scene manager
     */
    bool init();

    /**
     * @brief Frees all scene manager information
     */
    void shutdown();

    /**
     * @brief Sets the specified scene as active, if the new scene isn't fully loaded, then the engine defaults to the loading screen
     * @param scene New active scene
     */
    void setActive(Scene* scene);

    /**
     * @brief Returns the current active scene or nullptr if no active scene
     */
    Scene* getCurrentScene() const;

private:
    SceneManager() {}
    friend Engine;

private:
    Scene* m_activeScene = nullptr;
};

} // namespace scene
} // namespace aderite
