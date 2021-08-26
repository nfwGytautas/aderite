#pragma once

#include <string>

#include "aderiteeditor/utility/Macros.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/asset/Forward.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

/**
 * @brief Event router sink, containing functions that are invoked from editor components
*/
class IEventSink {
public:
	virtual ~IEventSink() {}

	/**
	 * @brief Invoked when the current selected Entity is changed
	*/
	virtual void onSelectedEntityChanged(scene::Entity& Entity) = 0;

	/**
	 * @brief Creates a new project
	 * @param dir Directory where the project will be created
	 * @param name Name of the project
	*/
	virtual void onNewProject(const std::string& dir, const std::string& name) = 0;

	/**
	 * @brief Save the project
	*/
	virtual void onSaveProject() = 0;

	/**
	 * @brief Load the specified project
	 * @param path Path to project file
	*/
	virtual void onLoadProject(const std::string& path) = 0;

	/**
	 * @brief Create new Scene
	 * @param name Name of the Scene
	*/
	virtual void onNewScene(const std::string& name) = 0;

	/**
	 * @brief Create new Entity for current active Scene
	 * @param name Name of the Entity
	*/
	virtual void onCreateEntity(const std::string& name) = 0;

	/**
	 * @brief Destroy Entity from current active Scene
	 * @param Entity Entity to destroy
	*/
	virtual void onDestroyEntity(const scene::Entity& Entity) = 0;

	/**
	 * @brief Invoked when the selected asset has been changed
	 * @param asset New selected asset
	*/
	virtual void onSelectedAssetChanged(asset::Asset* asset) = 0;

	/**
	 * @brief Invoked when the game should be stopped
	*/
	virtual void onStopGame() = 0;

	/**
	 * @brief Invoked when the game should be started
	*/
	virtual void onStartGame() = 0;

	/**
	 * @brief Invoked when the game state should be reset to that which was when starting the game
	*/
	virtual void onResetGameState() = 0;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
