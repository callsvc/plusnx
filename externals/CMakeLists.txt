cmake_minimum_required(VERSION 3.29)

set(VULKAN_HEADERS_ENABLE_TESTS OFF CACHE BOOL "Test Vulkan-Headers" FORCE)
set(VULKAN_HEADERS_ENABLE_INSTALL OFF CACHE BOOL "Install Vulkan-Headers" FORCE)

if(NOT TARGET Vulkan::Headers)
	add_subdirectory(Vulkan-Headers)
endif()

add_subdirectory(boost)

set(ENABLE_PROGRAMS OFF CACHE BOOL "Build Mbed TLS programs." FORCE)
set(MBEDTLS_FATAL_WARNINGS OFF CACHE BOOL "Compiler warnings treated as errors" FORCE)
add_subdirectory(mbedtls)
