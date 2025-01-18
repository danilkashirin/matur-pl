cmake_minimum_required(VERSION 3.26)

add_library(ast INTERFACE
        CompareOpNode.h
        ForNode.h
        IfNode.h
        FunctionAST.h)

target_include_directories(ast INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")