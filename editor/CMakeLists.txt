#========================================
# editor
#========================================

file(GLOB_RECURSE CPP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Exe mode
add_executable(aderite_editor ${CPP_SOURCE_FILES})

# Lib folders
if(BUILD_RELEASE)
    target_link_directories(aderite_editor PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/lib/release/)
else()
	message(STATUS "Linking debug libraries for editor")
	target_link_directories(aderite_editor PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/lib/debug/)
endif()

# Includes
target_include_directories(aderite_editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(aderite_editor PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/include/)
target_include_directories(aderite_editor PUBLIC ${PROJECT_SOURCE_DIR}/runtime/src/)

set_property(TARGET aderite_editor PROPERTY CXX_STANDARD 17)
set_property(TARGET aderite_editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Same name
target_link_libraries(aderite_editor PUBLIC aderite_runtime imgui)

# Some MSVC flags
set_property(TARGET aderite_editor PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")