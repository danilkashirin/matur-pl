cmake_minimum_required(VERSION 3.26)

add_library(llvm-backend STATIC IRGenerator.cpp JITExecutor.cpp)

find_package(LLVM 15.0.7 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(llvm-backend PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ast"
)

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

target_link_libraries(llvm-backend PUBLIC ${llvm_libs})