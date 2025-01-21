cmake_minimum_required(VERSION 3.26)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)

add_library(llvm-backend STATIC JITExecutor.cpp
        IRGeneratorV2.cpp
        ../vm/ASTToBytecodeConverter.cpp
        ../vm/VirtualMachine.cpp
)

target_include_directories(llvm-backend PUBLIC
        ${LLVM_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ast"
)

target_compile_definitions(llvm-backend PUBLIC ${LLVM_DEFINITIONS})

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