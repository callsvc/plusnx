cmake_minimum_required(VERSION 3.29)

add_executable(basic-jit-core-append armored/basic_jit_core_append.cpp)
target_link_libraries(basic-jit-core-append PRIVATE plusnx-core)

