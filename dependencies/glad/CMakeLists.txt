cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(glad)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include/)
