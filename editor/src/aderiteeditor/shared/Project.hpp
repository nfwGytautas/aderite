#pragma once

#include <filesystem>
#include <string>

#include "aderite/Handles.hpp"

#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Class containing all information about aderite project
 */
class Project {
public:
    /**
     * @brief Creates a new project object with name and base directory
     */
    Project(const std::string& dir, const std::string& name);
    virtual ~Project();

    /**
     * @brief Save project to file
     * @return True if saved without errors
     */
    bool save();

    /**
     * @brief Load project
     * @param path The project file
     * @return New project object if loaded successfully, nullptr otherwise
     */
    static Project* load(const std::string& path);

    /**
     * @brief Returns the project name
     */
    std::string getName() const;

    /**
     * @brief Returns the project root directory
     */
    std::filesystem::path getRootDir() const;

    /**
     * @brief Returns the linked FMOD project directory
     */
    std::filesystem::path getFmodProjectDir() const;

    /**
     * @brief Link FMOD project to aderite project
     * @param project Path to project Build path
     */
    void linkFmodProject(std::filesystem::path project);

    /**
     * @brief Returns the active scene of the project
     */
    io::SerializableHandle getActiveScene() const;

    /**
     * @brief Validates project information, for example if the current scene name changed the project
     * will automatically update it self to reflect these changes
     */
    void validate();

    /**
     * @brief Returns the virtual file system of the project
     */
    vfs::VFS* getVfs() const;

private:
    std::filesystem::path m_directory;
    std::filesystem::path m_fmodProjectDirectory;
    std::string m_name;
    io::SerializableHandle m_activeScene;

    vfs::VFS* m_vfs = nullptr;
};

} // namespace editor
} // namespace aderite
