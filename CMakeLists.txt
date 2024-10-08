cmake_minimum_required(VERSION 3.29)
project(plusnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE ON)

add_subdirectory(externals)

add_subdirectory(src)


