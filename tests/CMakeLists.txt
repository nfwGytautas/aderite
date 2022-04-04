cmake_minimum_required(VERSION 3.14)
project(aderite_test)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
	asset_test
	src/AssetTest.cpp
)

add_executable(
	io_test
	src/IoTest.cpp
)

add_executable(
	scene_test
	src/SceneTest.cpp
)

set(DEPENDENCIES aderite_runtime glfw3 mono-2.0-sgen libmono-static-sgen libmonoruntime-sgen fmodstudioL fmodL spdlogd yaml-cppd assimp-vc142-mtd zlibd bgfxDebug bimgDebug bxDebug LowLevel_static_64 LowLevelAABB_static_64 LowLevelDynamics_static_64 PhysX_64 PhysXCharacterKinematic_static_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64 PhysXTask_static_64 PhysXVehicle_static_64 SceneQuery_static_64 SimulationController_static_64)

set(INCLUDE_DIRS 
	${PROJECT_SOURCE_DIR}/../dependencies/include/ 
	${PROJECT_SOURCE_DIR}/../dependencies/include/physx/ 
	${PROJECT_SOURCE_DIR}/../dependencies/include/fmod/
	${PROJECT_SOURCE_DIR}/../dependencies/include/compat/msvc/
	${PROJECT_SOURCE_DIR}/../runtime/src/
)

target_include_directories(asset_test PUBLIC ${INCLUDE_DIRS})
target_link_directories(asset_test PUBLIC ${PROJECT_SOURCE_DIR}/../bin/)
target_link_directories(asset_test PUBLIC ${PROJECT_SOURCE_DIR}/../dependencies/windows/debug/)

target_include_directories(io_test PUBLIC ${INCLUDE_DIRS})
target_link_directories(io_test PUBLIC ${PROJECT_SOURCE_DIR}/../bin/)
target_link_directories(io_test PUBLIC ${PROJECT_SOURCE_DIR}/../dependencies/windows/debug/)

target_include_directories(scene_test PUBLIC ${INCLUDE_DIRS})
target_link_directories(scene_test PUBLIC ${PROJECT_SOURCE_DIR}/../bin/)
target_link_directories(scene_test PUBLIC ${PROJECT_SOURCE_DIR}/../dependencies/windows/debug/)

target_link_libraries(
	asset_test
	${DEPENDENCIES}
	gtest_main
	gmock_main
)

target_link_libraries(
	io_test
	${DEPENDENCIES}
	gtest_main
	gmock_main
)

target_link_libraries(
	scene_test
	${DEPENDENCIES}
	gtest_main
	gmock_main
)

# Copy dlls to output
file(GLOB_RECURSE DLL_FILES ${PROJECT_SOURCE_DIR}/../dependencies/windows/debug/*.dll)
file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR})

include(GoogleTest)
gtest_discover_tests(asset_test)
gtest_discover_tests(io_test)
gtest_discover_tests(scene_test)
