#========================================
# runtime
#========================================

file(GLOB_RECURSE CPP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Lib mode
add_library(aderite_runtime STATIC ${CPP_SOURCE_FILES})

# Lib folders
#target_link_directories(aderite_runtime PUBLIC ${DEPENDENCY_LIB_PATH})

# Includes
target_include_directories(aderite_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(aderite_runtime PUBLIC ${INCLUDE_DIRS})

set_property(TARGET aderite_runtime PROPERTY CXX_STANDARD 17)
set_property(TARGET aderite_runtime PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Libraries to link
if(BUILD_RELEASE)
    target_link_libraries(aderite_runtime PUBLIC ${PHYSX_LIBS} fmodstudio fmod spdlog yaml-cpp assimp-vc142-mt zlib bgfxRelease bimgRelease bxRelease)
else()
	target_link_libraries(aderite_runtime PUBLIC ${PHYSX_LIBS} fmodstudioL fmodL spdlogd yaml-cppd assimp-vc142-mtd zlibd bgfxDebug bimgDebug bxDebug)
endif()

# Same name
target_link_libraries(aderite_runtime PUBLIC glfw3 mono-2.0-sgen libmono-static-sgen libmonoruntime-sgen)

# Some MSVC flags
set_property(TARGET aderite_runtime PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")