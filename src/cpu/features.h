#pragma once

#include <types.h>
namespace Plusnx::Cpu {
    enum class ManufactureType {
        Invalid,
        Intel,
        Amd
    };
    class Features {
    public:
        Features();
        u32 GetCpuRank() const;

        ManufactureType type;
        std::string brand;

        u32 baseFrequencyHz;
        u32 maxFrequencyHz;

        u32 cores;

        i32 highest;
        i32 aes;
    private:
        void IdentifyCpuType(const std::array<u32, 4>& root);
        void GetCpuCount();
    };
}