#pragma once

/**
 * @brief This file contains all macros defined by the aderite engine and editor
 */

 // ---------------------------------
 // NAMESPACES
 // ---------------------------------

#include "aderite/utility/Macros.hpp"

 // Root namespace where all editor objects reside in
#define ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace editor {
#define ADERITE_EDITOR_ROOT_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Shared namespace where various functions and objects that are shared between multiple platforms is defined in
#define ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace shared {
#define ADERITE_EDITOR_SHARED_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Supported rendering backend namespace (bgfx)
#define ADERITE_EDITOR_BACKEND_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace backend {
#define ADERITE_EDITOR_BACKEND_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Component namespace where all components are defined in
#define ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace component {
#define ADERITE_EDITOR_COMPONENT_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Utility namespace where various utility functions are defined in
#define ADERITE_EDITOR_UTILITY_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace utility {
#define ADERITE_EDITOR_UTILITY_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }
