#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "aderite/Handles.hpp"
#include "aderite/io/SerializableObject.hpp"

#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace vfs {

/**
 * @brief Virtual File System (VFS) class provides the name mapping and virtual file system that the editor uses. Since
 * the runtime uses handles as file names, the editor provides a way to map strings to handles. This also allows the user
 * to have a file system with directories, etc. while the VFS takes care of mapping this to a single directory assets that
 * the runtime expects.
 */
class VFS final {
public:
    VFS();
    ~VFS();

    /**
     * @brief Finds files that contain the specified name
     * @param name Name to search for
     * @return File array
     */
    std::vector<File*> findFiles(const std::string& name) const;

    /**
     * @brief Get the file with specified path
     * @param path Path of the file
     * @return File instance or nullptr if path doesn't exist
     */
    File* getFile(const std::string& path) const;

    /**
     * @brief Returns the File instance associated with the handle
     * @param handle Handle of the asset
     * @return File instance
     */
    File* getFile(io::SerializableHandle handle) const;

    /**
     * @brief Get the directory with specified path
     * @param path Path of the directory
     * @return Directory instance or nullptr if path doesn't exist
     */
    Directory* getDirectory(const std::string& path) const;

    /**
     * @brief Returns the root directory instance
     */
    Directory* getRoot() const;

    /**
     * @brief Move a file to a directory, removes old file if it exists
     * @param target Directory to move to
     * @param file File to move
     */
    void move(Directory* target, File* file);

    /**
     * @brief Move a directory to a directory, removes old file if it exists
     * @param target Directory to move to
     * @param dir Directory to move
     */
    void move(Directory* target, Directory* dir);

    /**
     * @brief Remove file
     * @param file File to remove
     */
    void remove(File* file);

    /**
     * @brief Remove directory (INCLUDING ALL OF IT'S CONTENTS)
     * @param dir Directory to remove
     */
    void remove(Directory* dir);

    /**
     * @brief Renames a file
     * @param file File to rename
     * @param newName New name of the file
     */
    void rename(File* file, const std::string& newName);

    /**
     * @brief Renames a directory
     * @param dir Directory to rename
     * @param newName New name of the directory
     */
    void rename(Directory* dir, const std::string& newName);

    /**
     * @brief Saves the VFS to the current project root
     * @param root Root directory
     */
    void save(std::filesystem::path root) const;

    /**
     * @brief Loads the VFS from current project root
     * @param root Root directory
     */
    static VFS* load(std::filesystem::path root);

private:
    Directory* m_root = nullptr;
};

} // namespace vfs
} // namespace aderite
