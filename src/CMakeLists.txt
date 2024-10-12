cmake_minimum_required(VERSION 3.29)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(plusnx)
target_sources(plusnx PRIVATE
    video/gpu.cpp
    video/gpu.h
    video/vk/vulkan_types.h
    video/vk/instance.cpp
    video/vk/instance.h
    sys_fs/provider.cpp
    sys_fs/provider.h
    sys_fs/assets.cpp
    sys_fs/assets.h
    main.cpp
    application.cpp
    application.h
    context.cpp
    context.h
    types.h
)

target_link_libraries(plusnx PRIVATE Vulkan::Headers Vulkan::Vulkan SDL2)
