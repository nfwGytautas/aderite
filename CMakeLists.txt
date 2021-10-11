cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(aderite)

# Init
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

option(BUILD_RELEASE "If specified then release version of aderite will be built" OFF)

message(STATUS "Build release: " BUILD_RELEASE)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

set(PHYSX_LIBS LowLevel_static_64 LowLevelAABB_static_64 LowLevelDynamics_static_64 PhysX_64 PhysXCharacterKinematic_static_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64 PhysXTask_static_64 PhysXVehicle_static_64 SceneQuery_static_64 SimulationController_static_64)

message(STATUS "PhysX libs ${PHYSX_LIBS}")

add_subdirectory(runtime)
add_subdirectory(editor)

# Copy dlls to output
# Lib folders
if(BUILD_RELEASE)
    file(GLOB_RECURSE DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/release/*.dll)
else()
	message(STATUS "Copying debug dll files")
	file(GLOB_RECURSE DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/debug/*.dll)
endif()

message(STATUS "Copying dll files ${DLL_FILES}")

file(COPY ${DLL_FILES} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tools/ DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
