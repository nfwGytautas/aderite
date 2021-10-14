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

// Compiler namespace where various functions that involve creating assets such as material shader headers are stored in
#define ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace compiler {
#define ADERITE_EDITOR_COMPILER_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Node namespace where node editor related objects are defined in
#define ADERITE_EDITOR_NODE_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace node {
#define ADERITE_EDITOR_NODE_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Pipeline namespace where node editor related objects for rendering pipeline are defined in
#define ADERITE_EDITOR_PIPELINE_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace pipeline {
#define ADERITE_EDITOR_PIPELINE_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }

// Runtime namespace where extensions to the runtime are defined in
#define ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN ADERITE_EDITOR_ROOT_NAMESPACE_BEGIN namespace runtime {
#define ADERITE_EDITOR_RUNTIME_NAMESPACE_END ADERITE_EDITOR_ROOT_NAMESPACE_END }
