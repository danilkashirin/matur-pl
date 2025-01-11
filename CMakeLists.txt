cmake_minimum_required(VERSION 3.26)
project(matur_pl)

set(CMAKE_CXX_STANDARD 20)

# Установите CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/llvm@15/lib/cmake")

set(LLVM_DIR "${CMAKE_PREFIX_PATH}/llvm")
find_package(LLVM 15.0.7 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)

add_executable(matur_pl main.cpp
        llvm-backend/JITExecutor.h
        llvm-backend/JITExecutor.cpp
        llvm-backend/IRGenerator.h
        llvm-backend/IRGenerator.cpp)

target_include_directories(matur_pl PRIVATE "${PROJECT_SOURCE_DIR}/include")

llvm_map_components_to_libnames(llvm_libs
        core
        orcjit
        mcjit
        executionengine
        native
        support
        bitreader
        interpreter
        transformutils
)

target_link_libraries(matur_pl PRIVATE ${llvm_libs} parser ast)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)