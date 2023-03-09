cmake_minimum_required(VERSION 3.13)

project(cbc)

file(GLOB SRCS "*.cpp")

include_directories(../common/include)
include_directories(include)
add_executable(cbc ${SRCS})

target_compile_features(cbc PUBLIC cxx_std_17)
