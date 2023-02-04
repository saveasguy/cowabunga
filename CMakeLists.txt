cmake_minimum_required(VERSION 3.0)

project(kaleidoc)

file(GLOB SRCS "src/*.cc")
include_directories(src)
add_executable(kaleidoc ${SRCS})
