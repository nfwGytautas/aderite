#========================================
# editor
#========================================

file(GLOB_RECURSE CPP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl)

# Exe mode
add_executable(aderite_editor ${CPP_SOURCE_FILES})

# Lib folders
target_link_directories(aderite_editor PUBLIC ${DEPENDENCY_LIB_PATH})

# Includes
target_include_directories(aderite_editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(aderite_editor PUBLIC ${INCLUDE_DIRS})
target_include_directories(aderite_editor PUBLIC ${PROJECT_SOURCE_DIR}/runtime/src/)

set_property(TARGET aderite_editor PROPERTY CXX_STANDARD 17)
set_property(TARGET aderite_editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Same name
target_link_libraries(aderite_editor PUBLIC aderite_runtime imgui)

# Some MSVC flags
set_property(TARGET aderite_editor PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")