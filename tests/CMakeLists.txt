cmake_minimum_required(VERSION 3.29)

add_executable(basic-jit-core-append armored/basic_jit_core_append.cpp)
add_executable(rsassa-pss-list signatures/rsassa_pss_list.cpp)
add_executable(coroutines coroutines/main.cpp)
add_executable(statistics-details details/statistics.cpp)

target_link_libraries(basic-jit-core-append PRIVATE plusnx-core)
target_link_libraries(rsassa-pss-list PRIVATE plusnx-core)
target_link_libraries(coroutines PRIVATE plusnx-core)

target_link_libraries(statistics-details PRIVATE plusnx-core)

