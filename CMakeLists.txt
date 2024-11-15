cmake_minimum_required(VERSION 3.29)
project(plusnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-flto=auto -O3 -ffast-math -Werror -s")

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(plusnx)
add_library(plusnx-core)

add_subdirectory(externals)
add_subdirectory(tests)
add_subdirectory(tools)

add_subdirectory(src)


