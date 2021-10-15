#pragma once

#include <string>
#include <filesystem>
#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Block of data returned from resolve calls
 */
struct Path {
	// File to read from
	std::filesystem::path File;

	// Start byte, unused for YAML parser
	size_t Start = 0;

	// Number of bytes for this object, unused for YAML parser
	size_t Size = 0;
};

/**
 * @brief Utility class used to resolve a given path to physical data,
 * useful when working with relative paths or using a name to link to a file
*/
class PathResolver {
public:
	virtual ~PathResolver() {};

	/**
	 * @brief Resolve a given string value to a data block
	 * @param value String to resolve, can be a path or name or anything, depends on the resolver
	 * @return DataBlock
	*/
	virtual Path resolve(const std::string& value) = 0;

	/**
	 * @brief Resolve a given handle to it's path
	 * @param handle Handle of to resolve
	 * @return DataBlock
	*/
	virtual Path resolve(SerializableHandle handle) = 0;
};

}
}
