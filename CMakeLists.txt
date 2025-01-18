cmake_minimum_required(VERSION 3.26)
project(matur_pl)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/llvm@15/lib/cmake")

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(llvm-backend)
add_subdirectory(vm)

add_executable(matur_pl main.cpp)

target_include_directories(matur_pl PRIVATE "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(matur_pl PRIVATE parser ast llvm-backend vm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)