#pragma once
#include <platform/syncpoint_ctl.h>
namespace Plusnx::Platform {
    // https://http.download.nvidia.com/tegra-public-appnotes/host1x.html

    // The Tegra host1x module is the DMA engine for register access to Tegra’s graphics- and multimedia-related modules
    enum class ChannelType {
        Gpu,
        NvEnc, // NVIDIA Video Encoder
        NvDec, // NVIDIA Video Decoder
        Display,
        Vic,
        NvJpg // NVIDIA JPEG Decoder/Encoder
    };

    // https://github.com/torvalds/linux/blob/de2f378f2b771b39594c04695feee86476743a69/drivers/gpu/host1x/hw/debug_hw.c#L16
    enum class Host1xOpcodes {
        SetClass,
        Incr,
        NonIncr,
        Mask,
        Imm,
        Restart,
        Gather,
        SetStrMid,
        SetAppId,
        SetPyLd,
        IncrW,
        NonIncrW,
        GatherW,
        RestartW,
        Extend
    };

    class Host1x {
        SyncpointCtrl sync; // <! Stores all available buses on this device
    };
}