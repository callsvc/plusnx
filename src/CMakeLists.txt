cmake_minimum_required(VERSION 3.29)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(plusnx-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:>
)

target_sources(plusnx-core PUBLIC
    armored/arm_types.h
    armored/readable_text_block.cpp
    armored/readable_text_block.h
    armored/cpu_context.cpp
    armored/cpu_context.h
    armored/jit_context.cpp
    armored/jit_context.h
    armored/arm_x86_disas.h
    armored/backend/arm64_emitter_context.cpp
    armored/backend/arm64_emitter_context.h
    armored/backend/x86_64_emitter_context.cpp
    armored/backend/x86_64_emitter_context.h
    armored/backend/emitter_generator.cpp
    armored/backend/emitter_generator.h
    armored/backend/emitter_interface.h
    armored/frontend/arm64_translator.cpp
    armored/frontend/arm64_translator.h
    armored/frontend/info_target_instructions.cpp
    armored/frontend/info_target_instructions.h
    armored/frontend/arm64_faster_intrusive_decoder.cpp
    armored/frontend/arm64_faster_intrusive_decoder.h
    armored/ir/ir_descriptor_flow_graph.cpp
    armored/ir/ir_descriptor_flow_graph.h
    armored/ir/ir_types.h
    armored/ir/ir_types.cpp
    audio/output_device.cpp
    audio/output_device.h
    audio/input_device.cpp
    audio/input_device.h
    audio/output_device.cpp
    audio/output_device.h
    audio/device.cpp
    audio/device.h
    audio/sound.cpp
    audio/sound.h
    platform/hostx1.cpp
    platform/hostx1.h
    platform/syncpoint_ctl.cpp
    platform/syncpoint_ctl.h
    video/tegrax1_gpu.cpp
    video/tegrax1_gpu.h
    video/vk_render/app_instance.cpp
    video/vk_render/app_instance.h
    video/vk_render/graphics_device.cpp
    video/vk_render/graphics_device.h
    video/graphics_types.h
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
    sys_fs/nx/submission_package.cpp
    sys_fs/nx/submission_package.h
    sys_fs/nx/nca_core.cpp
    sys_fs/nx/nca_core.h
    sys_fs/nx/nso_core.cpp
    sys_fs/nx/nso_core.h
    sys_fs/nx/content_metadata.cpp
    sys_fs/nx/content_metadata.h
    sys_fs/extra/insertable_romfs.cpp
    sys_fs/extra/insertable_romfs.h
    sys_fs/extra/nso_modules.cpp
    sys_fs/extra/nso_modules.h
    sys_fs/layered_fs.cpp
    sys_fs/layered_fs.h
    sys_fs/streamed_file.cpp
    sys_fs/streamed_file.h
    sys_fs/meta_program.cpp
    sys_fs/meta_program.h
    sys_fs/ctr_backing.cpp
    sys_fs/ctr_backing.h
    input/app_controller.cpp
    input/app_controller.h
    input/sdl_inputs.cpp
    input/sdl_inputs.h
    input/controller_types.h
    core/application.cpp
    core/application.h
    core/context.cpp
    core/context.h
    core/games_lists.cpp
    core/games_lists.h
    core/telemetry_collector.cpp
    core/telemetry_collector.h
    core/configs.cpp
    core/configs.h
    core/process_qol.cpp
    core/process_qol.h
    cpu/core_blob.cpp
    cpu/core_blob.h
    cpu/features.cpp
    cpu/features.h
    cpu/coroutine.cpp
    cpu/coroutine.h
    cpu/kernel_task.cpp
    cpu/kernel_task.h
    cpu/unit_types.h
    nxk/base/auto_types.h
    nxk/base/k_auto_object.cpp
    nxk/base/k_auto_object.h
    nxk/base/k_tls_page.cpp
    nxk/base/k_tls_page.h
    nxk/base/k_recursive_lock.cpp
    nxk/base/k_recursive_lock.h
    nxk/base/k_process_handle_table.cpp
    nxk/base/k_process_handle_table.h
    nxk/memory/k_slab_heap.cpp
    nxk/memory/k_slab_heap.h
    nxk/types/kprocess.cpp
    nxk/types/kprocess.h
    nxk/types/kthread.cpp
    nxk/types/kthread.h
    nxk/types/kshared_memory.cpp
    nxk/types/kshared_memory.h
    nxk/svc/svc_types.h
    nxk/svc/svc_process.cpp
    nxk/svc/svc_process.cpp
    nxk/svc/svc_process.h
    nxk/kernel.cpp
    nxk/kernel.h
    nxk/user_space.cpp
    nxk/user_space.h
    nxk/address_space.cpp
    nxk/address_space.h
    nxk/memory_nx.cpp
    nxk/memory_nx.h
    nxk/os_allocator.cpp
    nxk/os_allocator.h
    nxk/tracker.cpp
    nxk/tracker.h
    nxk/constants.h
    nxk/scheduler_preemptive.cpp
    nxk/scheduler_preemptive.h
    loader/app_loader.cpp
    loader/app_loader.h
    loader/eshop_title.cpp
    loader/eshop_title.h
    loader/cartridge.cpp
    loader/cartridge.h
    loader/nx_executable.cpp
    loader/nx_executable.h
    loader/game_fs.cpp
    loader/game_fs.h
    loader/main_executable.cpp
    loader/main_executable.h
    loader/content_archive.cpp
    loader/content_archive.h
    os/nx_sys.cpp
    os/nx_sys.h
    os/process_creator.cpp
    os/process_creator.h
    os/sdk/control_property.cpp
    os/sdk/control_property.h
    os/make_loader.cpp
    os/make_loader.h
    services/settings/languages.cpp
    services/settings/languages.h
    recompiler/opcodes.cpp
    recompiler/opcodes.h
    security/checksum.cpp
    security/checksum.h
    security/cipher_cast.cpp
    security/cipher_cast.h
    security/key_types.cpp
    security/key_types.h
    security/keyring.cpp
    security/keyring.h
    security/signatures.cpp
    security/signatures.h
    security/ticket.cpp
    security/ticket.h
    details/perf_measure.cpp
    details/perf_measure.h
    details/allocators.h
    details/bits.h
    driver.cpp
    container.h
    runtime_exception.cpp
    runtime_exception.h
    types.cpp
    types.h
)

target_sources(plusnx PRIVATE
    main.cpp
    nogui/sdl_vulkan_backing.cpp
    nogui/sdl_vulkan_backing.h
    nogui/sdl_window.cpp
    nogui/sdl_window.h
)

target_link_libraries(plusnx-core PUBLIC Vulkan::Headers Vulkan::Vulkan SDL2::SDL2 Boost::align Boost::algorithm mbedcrypto toml11::toml11 sqlite3::sqlite3 lz4_static fmt::fmt tsl::robin_map)
if(PLUSNX_ENABLE_CAPSTONE)
    target_sources(plusnx-core PRIVATE
        armored/arm_x86_disas.cpp
    )
    target_link_libraries(plusnx-core PRIVATE capstone::capstone)
else()
    target_sources(plusnx-core PRIVATE armored/arm_x86_nullable.cpp)
endif()

target_link_libraries(plusnx PRIVATE plusnx-core Boost::program_options)
