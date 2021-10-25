#========================================
# script library
#========================================

file(GLOB_RECURSE CS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cs)

# DLL
add_library(aderite_scriptlib SHARED ${CS_SOURCE_FILES})

# Set the .NET Framework version for the executable.
set_property(TARGET aderite_scriptlib PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set(CMAKE_CSharp_FLAGS "/langversion:latest")
