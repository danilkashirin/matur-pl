cmake_minimum_required(VERSION 3.26)

add_library(lexer STATIC Lexer.cpp)

target_include_directories(lexer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")