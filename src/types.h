#pragma once
#include <cstdint>
#include <cassert>
#include <string>

#include <container.h>
namespace Plusnx {
    bool IsWaylandPresent();
    std::string GetSdlVersion();

    using u64 = std::uint64_t;

    using u32 = std::uint32_t;
    using i32 = std::int32_t;
}