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
 // HELPERS
 // ---------------------------------

// Helper define for logging when a unimplemented method is called
#define ADERITE_UNIMPLEMENTED LOG_WARN("Function {0} in {1}, on line {2} not yet implemented", __func__, __FILE__, __LINE__);

#ifdef _DEBUG
#define ADERITE_DEBUG_SECTION(code) code
#else
#define ADERITE_DEBUG_SECTION(code) 
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

// Physics namespace where all physics related functionality defined in
#define ADERITE_PHYSICS_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace physics {
#define ADERITE_PHYSICS_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Collider namespace where all colliders are defined in
#define ADERITE_COLLIDER_NAMESPACE_BEGIN ADERITE_PHYSICS_NAMESPACE_BEGIN namespace collider {
#define ADERITE_COLLIDER_NAMESPACE_END ADERITE_PHYSICS_NAMESPACE_END }

// Audio namespace where all audio related functionality is stored in
#define ADERITE_AUDIO_NAMESPACE_BEGIN ADERITE_ROOT_NAMESPACE_BEGIN namespace audio {
#define ADERITE_AUDIO_NAMESPACE_END ADERITE_ROOT_NAMESPACE_END }

// Pass namespace where various render passes are defined in
#define ADERITE_PASS_NAMESPACE_BEGIN ADERITE_RENDERING_NAMESPACE_BEGIN namespace pass {
#define ADERITE_PASS_NAMESPACE_END ADERITE_RENDERING_NAMESPACE_END }

// Uniform namespace where various uniforms are defined in
#define ADERITE_UNIFORM_NAMESPACE_BEGIN ADERITE_RENDERING_NAMESPACE_BEGIN namespace uniform {
#define ADERITE_UNIFORM_NAMESPACE_END ADERITE_RENDERING_NAMESPACE_END }

// Property namespace where all material type properties are defined in
#define ADERITE_PROPERTY_NAMESPACE_BEGIN ADERITE_ASSET_NAMESPACE_BEGIN namespace prop {
#define ADERITE_PROPERTY_NAMESPACE_END ADERITE_ASSET_NAMESPACE_END }
