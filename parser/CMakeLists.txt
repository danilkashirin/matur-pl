cmake_minimum_required(VERSION 3.26)

add_library(parser STATIC Parser.cpp)

target_include_directories(parser PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/ast"
)

target_link_libraries(parser PUBLIC ast lexer)