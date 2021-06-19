cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(imgui)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

file(GLOB_RECURSE CPP_SOURCE_FILES src/*.cpp include/imgui/*.h)
add_library(imgui STATIC ${CPP_SOURCE_FILES})
target_include_directories(imgui PUBLIC include/imgui/)
target_include_directories(imgui PUBLIC ../include/)
