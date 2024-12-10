#pragma once

#include <types.h>
namespace Plusnx::GenericKernel {
    u8* AllocatePages(u8* start, u64 size, i32 descriptor = -1, u64 offset = 0);

    void ProtectPages(u8* start, u64 size, bool read = true, bool write = true, bool execute = false);

    void FreePages(u8* pointer, u64 size, bool destroy = false);
}