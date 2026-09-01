cmake_minimum_required(VERSION 3.13)
project(LeptoInst)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

set(LEPTOINST_DEMO_BITCODE "${CMAKE_CURRENT_BINARY_DIR}/demo.bc")
add_custom_target(
  demo
  COMMAND ${LLVM_TOOLS_BINARY_DIR}/clang++
    -std=c++17
    -O1
    -fno-exceptions
    -fno-rtti
    -emit-llvm
    -c ${CMAKE_CURRENT_SOURCE_DIR}/examples/demo.cpp
    -o ${LEPTOINST_DEMO_BITCODE}
  COMMAND ${LLVM_TOOLS_BINARY_DIR}/opt
    -load-pass-plugin=$<TARGET_FILE:LeptoInst>
    -passes=lepto-inst
    -disable-output
    ${LEPTOINST_DEMO_BITCODE}
  DEPENDS LeptoInst
  USES_TERMINAL
  VERBATIM
)

# add_llvm_loadable_module(LeptoInst LeptoInst.cpp)
