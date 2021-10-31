#pragma once

// Version of the engine
// History:
//		0.0 - Initial release
constexpr const char* EngineVersion = "0.0";

// ---------------------------------
// WINDOWING BACKENDS
// Only one windowing backend can be active at a time
// 0 - Disabled
// 1 - Active
// ---------------------------------

// Is aderite using GLFW backend, for Windows/Linux/MacOS platforms
#define GLFW_BACKEND 1

// ---------------------------------
// RESOURCE SAVE TYPE
// Only one save type can be active at a time
// 0 - Disabled
// 1 - Active
// ---------------------------------

// Is aderite loading binary compressed files (Optimized for released games and applications)
#define BINARY_SAVE_TYPE 0

// Is aderite using some human readable type (Editors and debug builds)
#define HUMAN_READABLE_SAVE_TYPE 1

// ---------------------------------
// Other settings
// 0 - Disabled
// 1 - Active
// ---------------------------------

// Does the runtime get built with middleware capabilities
#define MIDDLEWARE_ENABLED 1

// Will the renderer support debug rendering
#define DEBUG_RENDER 1

// ---------------------------------
// Error checks
// ---------------------------------

#if GLFW_BACKEND == 0
#error "No backend specified"
#endif

#if BINARY_SAVE_TYPE == 1 && HUMAN_READABLE_SAVE_TYPE == 1
#error "Both binary and human readable save type enabled, only one can be enabled at a given time"
#elif BINARY_SAVE_TYPE == 0 && HUMAN_READABLE_SAVE_TYPE == 0
#error "Save type not specified"
#endif
