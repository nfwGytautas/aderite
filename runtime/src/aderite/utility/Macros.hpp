#pragma once

/**
 * @brief This file contains all macros defined by the aderite engine
 */

#include "aderite/Config.hpp"

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
#include <cassert>
#define ADERITE_DEBUG_SECTION(code)            code
#define ADERITE_STATIC_ASSERT(check, message)  static_assert(check, message)
#define ADERITE_DYNAMIC_ASSERT(check, message) assert(((void)message, check))
#else
#define ADERITE_DEBUG_SECTION(code)
#define ADERITE_STATIC_ASSERT(check, message)
#define ADERITE_DYNAMIC_ASSERT(check, message) LOG_ERROR("Failed check {0}, in {1} at line {2}, {3}", #check, __FILE__, __LINE__, message)
#endif

#if MIDDLEWARE_ENABLED == 1
#define ADERITE_MIDDLEWARE_SECTION(code) code
#define ADERITE_MIDDLEWARE_FINAL 
#define ADERITE_MIDDLEWARE_VIRTUAL virtual 
#else
#define ADERITE_MIDDLEWARE_SECTION(code)
#define ADERITE_MIDDLEWARE_FINAL final 
#define ADERITE_MIDDLEWARE_VIRTUAL 
#endif

#define ADERITE_STRINGIZE(var) #var
