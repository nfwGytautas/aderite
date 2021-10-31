cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(aderite LANGUAGES CXX CSharp)

# Init
set(ADERITE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ADERITE_OUTPUT_DIR}/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ADERITE_OUTPUT_DIR}/$<0:>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ADERITE_OUTPUT_DIR}/$<0:>)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

set(PHYSX_LIBS LowLevel_static_64 LowLevelAABB_static_64 LowLevelDynamics_static_64 PhysX_64 PhysXCharacterKinematic_static_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64 PhysXTask_static_64 PhysXVehicle_static_64 SceneQuery_static_64 SimulationController_static_64)

# TODO: Add check for compiler and platform
set(INCLUDE_DIRS 
	${PROJECT_SOURCE_DIR}/dependencies/include/ 
	${PROJECT_SOURCE_DIR}/dependencies/include/physx/ 
	${PROJECT_SOURCE_DIR}/dependencies/include/fmod/
	${PROJECT_SOURCE_DIR}/dependencies/include/compat/msvc/
)

# Options
option(BUILD_RELEASE "If specified then release version of aderite will be built" OFF)

message(STATUS "Build release: " ${BUILD_RELEASE})
message(STATUS "PhysX libs ${PHYSX_LIBS}")
message(STATUS "Include directories: ${INCLUDE_DIRS}")

# Some configuration for platforms
IF (WIN32)
  	set(DEPENDENCY_LIB_ROOT ${PROJECT_SOURCE_DIR}/dependencies/windows/)
ELSE()
	message(FATAL_ERROR "Non Windows platform is currently not supported (Build dependencies and modify CMakeLists to remove this message)")
ENDIF()

# Configuration setup
if(BUILD_RELEASE)
	set(DEPENDENCY_LIB_PATH ${DEPENDENCY_LIB_ROOT}/release/)
else()
	message(STATUS "Linking debug libraries")
	set(DEPENDENCY_LIB_PATH ${DEPENDENCY_LIB_ROOT}/debug/)
	add_compile_definitions(_DEBUG)
endif()

# Add projects
add_subdirectory(runtime)
add_subdirectory(editor)
add_subdirectory(scriptlib)

# Copy dlls to output
file(GLOB_RECURSE DLL_FILES ${DEPENDENCY_LIB_PATH}/*.dll)

message(STATUS "Copying dll files ${DLL_FILES}")

file(COPY ${DLL_FILES} DESTINATION ${ADERITE_OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${ADERITE_OUTPUT_DIR}/res/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tools/ DESTINATION ${ADERITE_OUTPUT_DIR}/tools/)
