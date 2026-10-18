cmake_minimum_required(VERSION 3.28)
project(SaturnAntiCheat C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 10.0.28000.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src/driver)
add_subdirectory(src/launcher)
