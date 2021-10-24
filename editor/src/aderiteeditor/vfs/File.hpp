#pragma once

#include <string>
#include "aderite/Handles.hpp"
#include "aderite/io/Forward.hpp"
#include "aderiteeditor/vfs/Forward.hpp"

namespace aderite {
namespace vfs {

/**
 * @brief Virtual file object
*/
class File final {
public:
	File(const std::string& name, io::SerializableHandle handle, Directory* parent = nullptr);

	/**
	 * @brief Returns the SerializableHandle associated with this file
	*/
	io::SerializableHandle getHandle() const;

	/**
	 * @brief Returns the parent of the directory, only the root directory will return nullptr here
	*/
	Directory* getParent() const;

	/**
	 * @brief Returns the name of the directory, empty string if root directory
	*/
	std::string getName() const;

	/**
	 * @brief Returns the full path to this file
	*/
	std::string getPath() const;
private:
	Directory* m_parent = nullptr;
	std::string m_name;
	io::SerializableHandle m_handle = c_InvalidHandle;

	friend class VFS;
};

}
}