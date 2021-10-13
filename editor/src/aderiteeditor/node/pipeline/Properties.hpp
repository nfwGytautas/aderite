#pragma once

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

ADERITE_EDITOR_PIPELINE_NAMESPACE_END
