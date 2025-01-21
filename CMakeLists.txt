cmake_minimum_required(VERSION 3.13)
project(LeptoInst)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(
  LeptoInstCompileCommands ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
)

include_directories(include)
add_subdirectory(src)
# add_llvm_loadable_module(LeptoInst LeptoInst.cpp)

