cmake_minimum_required(VERSION 3.29)
project(plusnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-flto=auto")

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(externals)

add_subdirectory(src)


