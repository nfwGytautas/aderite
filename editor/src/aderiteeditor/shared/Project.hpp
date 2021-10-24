#pragma once

#include <string>
#include <filesystem>
#include "aderite/Handles.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/Forward.hpp"
#include "aderiteeditor/vfs/Forward.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

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
	std::string m_name;
	io::SerializableHandle m_activeScene;

	vfs::VFS* m_vfs = nullptr;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
