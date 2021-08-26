#pragma once

/**
 * @brief This file contains all macros defined by the aderite engine
 */

 // ---------------------------------
 // PLATFORM
 // ---------------------------------

#ifdef _WIN32
 // Windows (x64 and x86)
#define ADERITE_PLATFORM_WINDOWS
#elif __unix__
 // Unix
#define ADERITE_PLATFORM_UNIX
#elif __linux__
 // linux
#define ADERITE_PLATFORM_LINUX
#elif __APPLE__
 // Mac OS
#define ADERITE_PLATFORM_MACOS
#else
#error "Unsupported platform"
#endif

// ---------------------------------
// NAMESPACES
// ---------------------------------

// Root namespace where all objects reside in
#define ADERITE_ROOT_NAMESPACE_BEGIN namespace aderite {
#define ADERITE_ROOT_NAMESPACE_END }

// Asset namespace where all asset related logic is defined in
#define ADERITE_ASSET_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace asset {
#define ADERITE_ASSET_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Interface namespace where all interfaces are stored in
#define ADERITE_INTERFACE_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace interfaces {
#define ADERITE_INTERFACE_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Rendering namespace where all rendering related logic is defined in
#define ADERITE_RENDERING_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace rendering {
#define ADERITE_RENDERING_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Scene namespace where all scene related logic is defined in
#define ADERITE_SCENE_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace scene {
#define ADERITE_SCENE_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Utility namespace where various random functionality is defined in
#define ADERITE_UTILITY_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace utility {
#define ADERITE_UTILITY_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Window namespace where all windowing related logic is defined in
#define ADERITE_WINDOW_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace window {
#define ADERITE_WINDOW_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Components namespace where all entity components are defined in
#define ADERITE_COMPONENTS_NAMESPACE_BEGIN ADERITE_SCENE_NAMESPACE_BEGIN namespace components {
#define ADERITE_COMPONENTS_NAMESPACE_END ADERITE_SCENE_NAMESPACE_END }

// Input namespace where all events and input objects are defined in
#define ADERITE_INPUT_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace input {
#define ADERITE_INPUT_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }
