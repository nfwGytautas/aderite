#pragma once

// Version of the engine
// History:
//		0.0.0 - Initial release
constexpr char* EngineVersion = "0.0.0";

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
// EDITOR ENABLED
// 0 - Disabled
// 1 - Enabled
// ---------------------------------

#define EDITOR_ENABLED 1
