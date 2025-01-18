cmake_minimum_required(VERSION 3.26)

add_library(vm STATIC LlvmParser.cpp)


target_include_directories(vm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")