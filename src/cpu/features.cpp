#include <cpuid.h>
#include <thread>

#include <boost/algorithm/string/trim.hpp>
#include <cpu/features.h>
#include <sys_fs/fsys/regular_file.h>

#if defined(__x86_64__)
void Plusnx::Cpu::Features::IdentifyCpuType(const std::array<u32, 4> &root) {
    std::string manufacture;
    manufacture.resize(12);

    std::memcpy(&manufacture[4 * 0], &root[1], 4);
    std::memcpy(&manufacture[4 * 1], &root[3], 4);
    std::memcpy(&manufacture[4 * 2], &root[2], 4);

    if (manufacture.contains("GenuineIntel"))
        type = ManufactureType::Intel;
    else if (manufacture.contains("AuthenticAMD"))
        type = ManufactureType::Amd;

    std::print("Capturing information about the host CPU: {}\n", manufacture);
}
void Plusnx::Cpu::Features::GetCpuCount() {
    auto count{std::thread::hardware_concurrency()};

    if (SysFs::FSys::RegularFile smt{"/sys/devices/system/cpu/smt/active"}) {
        if (smt.Read<i32>())
            count /= 2;
    }
    cores = count;
}

Plusnx::Cpu::Features::Features() {
    std::array<u32, 4> caps;
    highest = __get_cpuid_max(0, &caps[0]);

    __get_cpuid(0x80000000, &caps[0], &caps[1], &caps[2], &caps[3]);
    const u32 extended{caps.front()};

    __get_cpuid(0, &caps[0], &caps[1], &caps[2], &caps[3]);
    IdentifyCpuType(caps);
    GetCpuCount();

    __get_cpuid(1, &caps[0], &caps[1], &caps[2], &caps[3]);

    aes = (caps[3] & bit_AES) > 0;

    if (highest >= 0x10) {
        __get_cpuid(0x10, &caps[0], &caps[1], &caps[2], &caps[3]);
        baseFrequencyHz = caps[0];
        maxFrequencyHz = caps[1];
    }

    if (extended < 0x80000004)
        return;

    brand.resize(0x30);
    __get_cpuid(0x80000002, &caps[0], &caps[1], &caps[2], &caps[3]);
    std::memcpy(&brand[0], &caps[0], sizeof(caps));

    __get_cpuid(0x80000003, &caps[0], &caps[1], &caps[2], &caps[3]);
    std::memcpy(&brand[0x10], &caps[0], sizeof(caps));

    __get_cpuid(0x80000004, &caps[0], &caps[1], &caps[2], &caps[3]);
    std::memcpy(&brand[0x20], &caps[0], sizeof(caps));

    brand.resize(std::strlen(brand.data()));
    boost::trim_right(brand);
    brand.shrink_to_fit();
}
Plusnx::u32 Plusnx::Cpu::Features::GetCpuRank() const {
    u32 rank{};
    if (aes)
        rank += 100;

    return rank;
}
#endif
