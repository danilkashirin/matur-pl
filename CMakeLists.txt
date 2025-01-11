cmake_minimum_required(VERSION 3.26)
project(matur_pl)

set(CMAKE_CXX_STANDARD 20)

set(LLVM_DIR "/opt/homebrew/opt/llvm@15/lib/cmake/llvm")
find_package(LLVM 15.0.7 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(matur_pl main.cpp
        lexer.h
        parser.h
        ast.h
        llvm.h)

target_include_directories(matur_pl PRIVATE "${PROJECT_SOURCE_DIR}/include")

llvm_map_components_to_libnames(
        llvm_libs
        core
        orcjit
        mcjit
        executionengine # Базовая поддержка ExecutionEngine
        native
        support
        bitreader
        interpreter    # Опционально, если нужна базовая интерпретация
        transformutils # Включите утилиты для трансформации для JIT
)
target_link_libraries(matur_pl ${llvm_libs})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)