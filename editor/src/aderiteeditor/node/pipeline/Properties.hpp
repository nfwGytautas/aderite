#pragma once

#include <string>
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_PIPELINE_NAMESPACE_BEGIN

enum class PropertyType : size_t {
	Target = 0, // FBO
	Entity = 1, // Entity in the world that has components for rendering
	Camera = 2, // Camera provider
	Require = 3, // Require connection
	Eye = 4, // Lower level component of the camera
};

/**
 * @brief Returns the name for specified property type
 * @param type Type property type
 * @return Name string
*/
const char* getTypeName(PropertyType type);

/**
 * @brief Checks it the type is an array
 * @param type Type to check
 * @return True if array type
*/
bool isArray(const std::string& type);

/**
 * @brief Performs a check if the specified 'check' parameter is of type 'type' or an array of it
 * @param check Type to check
 * @param type Type to verify
 * @return True if array or type, false otherwise
*/
bool isArrayOrType(const std::string& check, const std::string& type);

ADERITE_EDITOR_PIPELINE_NAMESPACE_END
