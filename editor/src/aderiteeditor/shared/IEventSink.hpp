#pragma once

#include <string>

#include "aderite/io/Forward.hpp"

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Event router sink, containing functions that are invoked from editor components
 */
class IEventSink {
public:
    virtual ~IEventSink() {}

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

} // namespace editor
} // namespace aderite
