cmake_minimum_required(VERSION 3.29)
project(plusnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PLUSNX_ENABLE_ASAN "Enable memory sanitizer for the entire program (performance impact)" OFF)
option(PLUSNX_DISABLE_FAST_OPTION "Disable the -0fast optimization option" ON)
option(PLUSNX_ENABLE_CAPSTONE "Enable Capstone to assist with debugging" ON)
option(PLUSNX_REDUCE_CAPSTONE_SIZE "Enable Capstone's Diet mode" OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "-flto=auto -s")

if(PLUSNX_ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()
if(PLUSNX_DISABLE_FAST_OPTION)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
endif()

set(TOML11_PRECOMPILE ON CACHE BOOL "precompile toml11 library" FORCE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(PLUSNX_ENABLE_CAPSTONE OFF CACHE BOOL "Enable Capstone to assist with debugging" FORCE)
endif()

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)
find_package(fmt 11 REQUIRED)
find_package(toml11 REQUIRED)

add_library(plusnx-core)
add_executable(plusnx)

add_subdirectory(src)
add_subdirectory(externals)
add_subdirectory(tests)
add_subdirectory(tools)

