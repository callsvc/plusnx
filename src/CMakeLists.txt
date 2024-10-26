cmake_minimum_required(VERSION 3.29)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(plusnx)
target_sources(plusnx PRIVATE
    audio/speaker.cpp
    audio/speaker.h
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
    sys_fs/fs_types.h
    sys_fs/fs_types.cpp
    sys_fs/fsys/rigid_directory.cpp
    sys_fs/fsys/rigid_directory.h
    sys_fs/fsys/regular_file.cpp
    sys_fs/fsys/regular_file.h
    sys_fs/nx/partition_filesystem.cpp
    sys_fs/nx/partition_filesystem.h
    sys_fs/nx/readonly_filesystem.cpp
    sys_fs/nx/readonly_filesystem.h
    sys_fs/bounded.cpp
    sys_fs/bounded.h
    nogui/sdl_vulkan_backing.cpp
    nogui/sdl_vulkan_backing.h
    core/application.cpp
    core/application.h
    core/context.cpp
    core/context.h
    core/games_lists.cpp
    core/games_lists.h
    cpu/core_container.cpp
    cpu/core_container.h
    cpu/unit_types.h
    gsk/types/kprocess.cpp
    gsk/types/kprocess.h
    gsk/types/kthread.cpp
    gsk/types/kthread.h
    gsk/kernel.cpp
    gsk/kernel.h
    loader/app_loader.cpp
    loader/app_loader.h
    loader/eshop_title.cpp
    loader/eshop_title.h
    loader/cartridge.cpp
    loader/cartridge.h
    loader/nx_executable.cpp
    loader/nx_executable.h
    os/nx_sys.cpp
    os/nx_sys.h
    main.cpp
    driver.cpp
    container.h
    types.cpp
    types.h
)

target_link_libraries(plusnx PRIVATE Vulkan::Headers Vulkan::Vulkan SDL2 Boost::align)
