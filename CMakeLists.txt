cmake_minimum_required(VERSION 3.29)
project(plusnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PLUSNX_ENABLE_ASAN "Enable memory sanitizer for the entire program (performance impact)" OFF)
option(PLUSNX_DISABLE_FAST_OPTION "Disable the -0fast optimization option" ON)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "-flto=auto -Werror -s")

if(PLUSNX_ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()
if(PLUSNX_DISABLE_FAST_OPTION)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
endif()

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(plusnx)
add_library(plusnx-core)

add_subdirectory(externals)
add_subdirectory(tests)
add_subdirectory(tools)

add_subdirectory(src)

file(COPY ${CMAKE_SOURCE_DIR}/Plusnx.supp DESTINATION ${CMAKE_BINARY_DIR})


