#pragma once

#include <types.h>
namespace Plusnx::Input {
#pragma pack(push, 1)
    struct HidMemoryLayout {
        std::array<u8, 0x400> debugPad;
        std::array<u8, 0x3000> touchScreen;
        std::array<u8, 0x400> mouse;
        std::array<u8, 0x400> keyboard;
        std::array<u8, 0x1000> digitizer;
        std::array<u8, 0x200> homeButton;
        std::array<u8, 0x200> sleepButton;
        std::array<u8, 0x200> captureButton;
        std::array<u8, 0x800> inputDetector;
        [[deprecated]] std::array<u8, 0x4000> uniquePad; // [5.0.0+] This section has been removed from #SharedMemoryFormat. #UniquePadSharedMemoryFormat is now a specialization of #NpadSharedMemoryFormat
        std::array<u8, 0x32000> nPad;
        std::array<u8, 0x800> gesture;
        std::array<u8, 0x20> consoleSixAxisSensor;

        std::array<u64, 0x7BC> pad0;
    };
#pragma pack(pop)
    static_assert(sizeof(HidMemoryLayout) == 0x40000);

    class FrontendUserControllerDriver {
    public:
        virtual ~FrontendUserControllerDriver() = default;

        virtual void PickAJoystick() = 0;
    };
}