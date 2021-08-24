#pragma once

#include <vector>

#include "aderite/utility/macros.hpp"
#include "aderite/scene/Forward.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

/**
 * @brief Scene manager for aderite
*/
class SceneManager {
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
	 * @brief Sets the specified scene as active, if the new scene isn't loaded yet than this call will block until it is loaded
	 * @param scene New active scene
	*/
	void set_active(Scene* scene);

	/**
	 * @brief Returns the current active scene or nullptr if no active scene
	*/
	Scene* current_scene() const;
private:
	SceneManager() {}
	friend class engine;

private:
	Scene* m_activeScene = nullptr;
};

ADERITE_SCENE_NAMESPACE_END
