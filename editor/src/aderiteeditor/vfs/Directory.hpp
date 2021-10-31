#pragma once

#include <string>
#include <vector>

#include "aderite/Handles.hpp"

#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace vfs {

/**
 * @brief Virtual directory object, used to emulate directories
 */
class Directory final {
public:
    Directory(const std::string& name, Directory* parent = nullptr);
    ~Directory();

    /**
     * @brief Returns the parent of the directory, only the root directory will return nullptr here
     */
    Directory* getParent() const;

    /**
     * @brief Returns the name of the directory, empty string if root directory
     */
    std::string getName() const;

    /**
     * @brief Returns the file in this directory
     * @param name Name of the file
     * @return File instance
     */
    File* getFile(const std::string& name);

    /**
     * @brief Checks if the directory has a file with the specified name
     * @param name Name of the file
     * @return True if a file exists, false otherwise
     */
    bool hasFile(const std::string& name) const;

    /**
     * @brief Returns the directory in this directory
     * @param name Name of the directory
     * @return Directory instance
     */
    Directory* getDirectory(const std::string& name);

    /**
     * @brief Checks if the directory has a directory with the specified name
     * @param name Name of the directory
     * @return True if a directory exists, false otherwise
     */
    bool hasDirectory(const std::string& name) const;

    /**
     * @brief Creates a Directory
     * @param name Name of the Directory
     * @return Directory instance
     */
    Directory* createDirectory(const std::string& name);

    /**
     * @brief Returns the directories in this directory
     */
    const std::vector<Directory*> getDirectories() const;

    /**
     * @brief Returns the files in this directory
     */
    const std::vector<File*> getFiles() const;

    /**
     * @brief Returns the full path to this directory
     */
    std::string getPath() const;

private:
    Directory* m_parent = nullptr;
    std::string m_name = "";

    std::vector<File*> m_files;
    std::vector<Directory*> m_directories;

    friend class VFS;
    friend class File;
};

} // namespace vfs
} // namespace aderite
