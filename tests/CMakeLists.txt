cmake_minimum_required(VERSION 3.29)

add_executable(basic-jit-core-append armored/basic_jit_core_append.cpp)
add_executable(rsassa_pss_list signatures/rsassa_pss_list.cpp)
target_link_libraries(basic-jit-core-append PRIVATE plusnx-core)
target_link_libraries(rsassa_pss_list PRIVATE plusnx-core)

