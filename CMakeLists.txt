cmake_minimum_required(VERSION 3.13)

project(cowabunga)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(COWABUNGA_INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/include)

include_directories(include)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(examples)
