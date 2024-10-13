cmake_minimum_required(VERSION 3.29)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(plusnx)
target_sources(plusnx PRIVATE
    video/gpu.cpp
    video/gpu.h
    video/vk/vulkan_types.cpp
    video/vk/vulkan_types.h
    video/vk/instance.cpp
    video/vk/instance.h
    sys_fs/provider.cpp
    sys_fs/provider.h
    sys_fs/assets.cpp
    sys_fs/assets.h
    nogui/sdl_vulkan_backing.cpp
    nogui/sdl_vulkan_backing.h
    main.cpp
    driver.cpp

    application.cpp
    application.h
    context.cpp
    context.h
    container.h
    types.h
)

target_link_libraries(plusnx PRIVATE Vulkan::Headers Vulkan::Vulkan SDL2)
